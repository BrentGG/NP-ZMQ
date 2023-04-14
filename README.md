# Network Programming - ZMQ Service

## General

This project contains a ZMQ service called The Oasis, which is a virtual casino where clients can play a number of casino games. Clients must register once and then log in every time they wish to start a session. The client accounts, which includes their username, password, and credit amount, are stored in an SQLite database.

The following sections describe a diagram of the service, client and Benternet, as well as the requests that can be made and responses to be expected.

## Diagram

![Diagram of the Benternet and the Oasis service and client](./diagram.png?raw=true)

The Oasis service communicates with the Benternet over ZMQ. The Oasis clients in turn also communicate with the Benternet over ZMQ.

## Requests and Responses

