# Finding OpenSSL hostname verification bugs

These queries help with bug hunting the [SSL-Conservatory style](https://github.com/iSECPartners/ssl-conservatory) bugs.

## Missing verify callback

A SSL/TLS client can choose from several server certificate verification strategies. One required step is to verify the certificate CN/SAN matches the intended host.

A common way to implement this checking is with a callback set with `SSL_CTX_set_verify` or `SSL_CTX_set_cert_verify_callback`. Thus if `SSL_CTX_set_verify` is used without a callback set it is possible hostname validation does not occur.

False positives include hostname verification strategies where the peer certificate is inspected before sending data over the socket.

## Missing verify callback within Boost ASIO

This is very similar to the above situation. Boost's ASIO library wraps several OpenSSL APIs. Without using `set_verify_callback` the code most likely fails to verify the server certificate hostname.

The percision of this query is much higher.

# Verify callback preverified ignored

Finally in the verify callback method mentioned above, one parameter is a `preverified` result. This is the result of OpenSSL checking various x509 fields, such as expiry time, and verifying a trust chain from leaf cert to a root configured for the context.

A callback can still be correct and not use this value, for example it can reimplement the verification.

There are lots of instances where this function body looks like `{ return 1; }` meaning it ignores the pre-verification and also does not check the hostname.
