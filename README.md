# 📡 TFTP Client-Server File Transfer


## 📌 Description

This project implements a TFTP Client and Server that communicate over the **UDP protocol** to transfer files between two systems. The application supports **GET (Read Request)** and **PUT (Write Request)** operations by exchanging TFTP packets such as **RRQ, WRQ, DATA, ACK, and ERROR**. The implementation follows the TFTP communication flow, including block-wise data transfer, acknowledgments, and error handling.

The project provides practical experience with **Computer Networks**, **Linux Socket Programming**, and **System Programming**.

---

## ✨ Features

- Client-Server architecture using UDP sockets
- File download (GET/RRQ)
- File upload (PUT/WRQ)
- Supports Binary (Octet) transfer mode
- TFTP packet creation and parsing
- DATA packet transmission in 512-byte blocks
- ACK packet generation and validation
- Block number management
- Error packet handling
- File read/write using standard C file APIs
- Interactive command-line client
- Graceful client disconnection
- Server capable of handling multiple transfer requests sequentially

---

## 🛠 Technologies Used

- **Language:** C
- **Operating System:** Linux (Ubuntu)
- **Protocol:** UDP
- **Networking:** Berkeley Socket API
- **Compiler:** GCC
- **Editor:** VS Code / Vim
- **Version Control:** Git & GitHub

### Linux APIs Used

- `socket()`
- `bind()`
- `sendto()`
- `recvfrom()`
- `close()`
- `fopen()`
- `fread()`
- `fwrite()`
- `fclose()`
- `memcpy()`
- `memset()`

---

## ✅ Validation

| Test Case | Status |
|-----------|--------|
| Client connects to server | ✅ |
| GET request (RRQ) | ✅ |
| PUT request (WRQ) | ✅ |
| File upload | ✅ |
| File download | ✅ |
| ACK packet exchange | ✅ |
| DATA packet transmission | ✅ |
| Invalid filename handling | ✅ |
| File not found error | ✅ |
| Binary file transfer | ✅ |
| Graceful client disconnect | ✅ |

---

## 🚀 How to Run

### Compile

#### Server

```bash
gcc server.c -o server
```

#### Client

```bash
gcc client.c -o client
```

---

### Start the Server

```bash
./server
```

Example:

```text
TFTP Server listening on port 6969...
Waiting for client requests...
```

---

### Start the Client

Open another terminal.

```bash
./client
```

---

### Available Commands

```text
connect <server_ip>

get <filename>

put <filename>

mode octet

quit
```

---

## 💻 Sample Output

### Server

```text
TFTP Server listening on port 6969...

Waiting for client request...

Received RRQ for sample.txt

Sending DATA Block 1

ACK Received for Block 1

Sending DATA Block 2

ACK Received for Block 2

File Transfer Completed Successfully.
```

### Client

```text
tftp> connect 127.0.0.1

Connected Successfully.

tftp> get sample.txt

Receiving File...

Received Block 1

ACK Sent

Received Block 2

ACK Sent

File Downloaded Successfully.

tftp> put test.txt

Uploading File...

Sending Block 1

ACK Received

Sending Block 2

ACK Received

File Uploaded Successfully.

tftp> quit

Disconnected from Server.
```

---



## 👩‍💻 Author

**Praveena Vasu**

