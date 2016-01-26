Keywatch
========
**Automatic key  change detection for PGP**

Public keyservers are inherently untrustworthy: anyone can submit a key,
and even servers such as the PGP Global Directory, which provides email
verification, can only be as secure as the email verification process.

Ideally we would all use the web of trust, however this does not occur
very often in practice.  If PGP is used at all, in many cases it is used
with a key downloaded directly from a website, or&mdash;more
precariously&mdash;from a keyserver.

Keywatch audits these sources in order to prevent malicious users or
servers from providing incorrect keys.  Should a malicious user
submit a new key to the keyserver, the true user will be made
aware.  A unique feature of Keywatch is the ability to act against
malicious _keyservers_ as well.  By connecting to the keyserver
using Tor, it is possible to provide a conditional guarantee that the
keyserver is not selectively providing different keys to different
users.