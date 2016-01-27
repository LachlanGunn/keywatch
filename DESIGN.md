Keywatch: System Design
===========================

This document is intended to discuss the design of the Keywatch software.
It provides some background material intended to aid understanding of the
the design decisions involved, but also describes in some detail the
structure of the software.

Background
----------

The idea of Keywatch arose from the author's frustration at the difficulty
of embedding himself into the PGP web of trust.  As an interim measure,
he placed his key onto his website and into its DNS records, however this
fails to provide any guarantee of identity greater than the security of his
server and that of the CA system.

In order to overcome this limitation, the idea was formed to periodically
request a copy of the key from the web server in order to make sure that
it had not been compromised.  This may protect against certain attackers,
but what if they respond with to the auditor with a different key?  The
auditor must be indistinguishable from other clients.

The solution, then, is to proxy the requests so that an attacker cannot
distinguish the requests from one another.  Clearly, the proxy must be
located on a different machine, but under whose control?  If the client,
then the server can use fingerprinting.  If the server, then a malicious
operator can distinguish the clients.

It is therefore necessary that the proxy be run by a trusted third party,
and for this we propose to use the Tor network.  We believe that this is
sufficient due to the diversity of relay operators and open nature of
the Tor software.  Each request is routed through a new Tor circuit,
ideally rendering the auditor indistinguishable from other clients
using similar software.

System Scope
------------

We begin by describing the scope of the Keywatch system.  Not all of these
components will initially be implemented, and initial work focusses in
particular upon _keywatchd_.

### keywatchd

This daemon watches an HKP keyserver for changes in the fingerprint of the
first listed key for an email address.  As the HKP protocol does not give
any guarantees on the order in which keys will be returned, this is merely
wild guess as to the proper choice of key based on the observed behaviour
of http://sks-keyservers.net/.

When a change is detected from the previously-observed or known-good value
of the key, the user is warned by some means.  For the moment this means
an entry on the command-line, however this will be rendered more usable
later in the development process.

Implementation structure
------------------------

The _keywatch_ implementation has been split into the following modules:

 * daemon: provides _keywatchd_-specific functionality.
 * hkp: facilitates communication with HKP-based keyservers.
 * keys: structures describing PGP keys.

### daemon

The _daemon_ module is responsible for the business of downloading and
checking the keys.  It spawns a thread for each identity being checked,
with each thread downloading the associated list of keys at regular
intervals.  In order to prevent the server from fingerprinting users based
on the time of their requests, the requests are made in lock-step at
integer multiples of the interval time starting from 2000-01-01T0000Z.
In order to prevent errors in a user's clock from identifying them to
the server, the exact time of the request is selected at random from
within this interval.

### hkp

The HKP module is responsible for communication with the keyserver.  The
actual communication takes place using _libcurl_ and Tor.  In order to
trigger the creation of a new circuit for each request, the authentication
credentials supplied to Tor are modified each time to take the form
"XXX:password", where _XXX_ is a global counter that is incremented on
every request.

Parsing of the responses is performed using the _boost::spirit_ library.

### keys

Keys are currently represented according to the information provided by
the HKP protocol.  A _PublicKey_ object contains a fingerprint, algorithm type
and key length, creation and expiration dates, a free-form _flags_ string,
along with an array of user IDs associated with it.  A _UserID_ object
contains an identifier along with the creation and expiration dates, and
a flags string.