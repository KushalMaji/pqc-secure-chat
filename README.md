# Post-Quantum Secure Chat using ML-KEM-768

## Overview

This project implements a secure client-server chat application using **Post-Quantum Cryptography (PQC)**. It demonstrates how a quantum-resistant key encapsulation mechanism can be used to establish a secure communication channel between two parties.

The project uses **ML-KEM-768 (Kyber)** from the Open Quantum Safe (liboqs) library to perform post-quantum key exchange. The shared secret generated during the key exchange is hashed using SHA-256 to derive a 256-bit AES session key, which is then used to encrypt and decrypt chat messages.

---

## Features

- Post-Quantum Key Exchange using ML-KEM-768
- AES-256 encrypted messaging
- SHA-256 session key derivation
- TCP client-server communication
- Secure message encryption and decryption
- Error handling for sockets and cryptographic operations
- Implemented in C using liboqs and OpenSSL

---

## Technologies Used

- C Programming
- liboqs (Open Quantum Safe)
- OpenSSL
- TCP Sockets
- Linux (Ubuntu/SEED VM)

---

## Project Structure

```
.
├── client.c
├── server.c
├── crypto.c
├── crypto.h
├── common.h
├── Makefile
├── README.md
└── .gitignore
```

---

## Cryptographic Workflow

1. Client generates an ML-KEM-768 key pair.
2. Client sends its public key to the server.
3. Server performs ML-KEM encapsulation.
4. Server sends the ciphertext to the client.
5. Client performs ML-KEM decapsulation.
6. Both client and server derive the same shared secret.
7. SHA-256 derives a 256-bit AES session key.
8. Messages are encrypted using AES-256 before transmission.
9. Received messages are decrypted using the shared session key.

---

## Building the Project

Compile using:

```bash
make
```

or

```bash
gcc server.c crypto.c -loqs -lssl -lcrypto -o server
gcc client.c crypto.c -loqs -lssl -lcrypto -o client
```

---

## Running

Start the server:

```bash
./server
```

In another terminal, start the client:

```bash
./client
```

Type messages on either side to communicate securely.

Type:

```
exit
```

to terminate the chat.

---

## Security Features

- Quantum-resistant key exchange (ML-KEM-768)
- SHA-256 key derivation
- AES-256 symmetric encryption
- Encrypted message transmission
- Session key generated dynamically for every connection

---

## Screenshots

<img width="959" height="539" alt="wait" src="https://github.com/user-attachments/assets/a2a3367d-0d93-458f-a496-59e78607c0e1" />
<img width="959" height="539" alt="client" src="https://github.com/user-attachments/assets/3586f973-d10d-45d8-a5e0-0136614d517e" />
<img width="959" height="539" alt="connection_established" src="https://github.com/user-attachments/assets/798734c4-13b7-4b24-bce1-e7e2743511f2" />
<img width="959" height="535" alt="mssg_server" src="https://github.com/user-attachments/assets/4d391a1c-e4ff-4fd1-ba57-b794f3b9f2b2" />
<img width="959" height="539" alt="mssg_client" src="https://github.com/user-attachments/assets/0fd6ad40-ecc1-4f28-86fc-8fa8851491f0" />

---

## Future Improvements

- Digital signatures using ML-DSA
- Random IV generation for every message
- AES-GCM authenticated encryption
- Multi-client support
- Certificate-based authentication
- Protection against replay attacks

---

## Author

**Kushal Maji**

Cybersecurity Project

Post-Quantum Secure Communication using ML-KEM-768
