# One-Time Pad Encryption Software
This project was done to explore networking in C. It uses a client-server relationship to encrypt and decrypt messages fed into the client using one-time pad ecryption.

## Using the software:
First, generate a randomized key using keygen (Note: keys must be longer than the document that is to be encrypted):

`./keygen *key length* > mykey`

Next, get both servers running by entering:

`./enc_server *port number* &`

`./dec_server *port number* &`

Now that the servers are running, and your key is generated, it's ready to accept a document for encryption. All text must be upper case, symbols or lower case will result in an error message.

here is an example of an encryption call:

`./enc_client plaintext1 mykey *port number* > ciphertext`

I have included 5 examples of plaintext documents which are made to showcase the project.

To unencrypt a document, simply call the encrypted document using dec_client.

`./dec_client ciphertext mykey *port number* > plaintext1a`
