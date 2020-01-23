			
# Weak (duplicated) encryption keys for ASP.NET Telerik

## Overview

ASP.NET Telerik upload allows developers to easily
manage file uploads. The transmission between the client and the
server must be encrypted and impossible to decode, so the data cannot
be used by a malicious entity in an attack against the server. The
main security recommendation for Telerik is setting custom unique
strong random values for `Telerik.AsyncUpload.ConfigurationEncryptionKey`
and `Telerik.Upload.ConfigurationHashKey`.

The CodeQL query detects applications that are using the same key 
for both fields while they should have been unique

## Recommendation
Set a custom unique strong random value for
`Telerik.AsyncUpload.ConfigurationEncryptionKey`.

Set a custom unique strong random value for
`Telerik.Upload.ConfigurationHashKey`.

## Example

The following example shows a secure configuration for Telerik Upload
in the file `Web.config`.
```
<appSettings>
        <add key="Telerik.AsyncUpload.ConfigurationEncryptionKey" value="YOUR-FIRST-UNIQUE-STRONG-RANDOM-VALUE-UNIQUE-TO-YOUR-APP" />
        <add key="Telerik.Upload.ConfigurationHashKey" value="YOUR-SECOND-UNIQUE-STRONG-RANDOM-VALUE-UNIQUE-TO-YOUR-APP" />
        <add key="Telerik.Upload.AllowedCustomMetaDataTypes" value="Telerik.Web.UI.AsyncUploadConfiguration" />
</appSettings>
```

## References
- Telerik: [Security Recommendations](https://docs.telerik.com/devtools/aspnet-ajax/controls/asyncupload/security).
- Telerik: [Cryptographic Weakness](https://www.telerik.com/support/kb/aspnet-ajax/details/cryptographic-weakness).
- Exploitation: [Pwning Web Applications via Telerik WebUI](https://captmeelo.com/pentest/2018/08/03/pwning-with-telerik.html).

