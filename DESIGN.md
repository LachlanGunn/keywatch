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