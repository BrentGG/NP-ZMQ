# Network Programming - ZMQ Service

## General

This project contains a ZMQ service called The Oasis, which is a virtual casino where clients can play a number of casino games. Clients must register once and then log in every time they wish to start a session. New clients start with 1000 credits. The client accounts, which includes their username, password, and credit amount, are stored in an SQLite database. Clients are automatically logged out after they've been inactive for at least 15 minutes.

The 'Diagram' section shows a diagram of the service, client and Benternet. The 'Resuests and Responses' sections describes all the available and planned requests and responses, as well as information on how to use them.

## Diagram

![Diagram of the Benternet and the Oasis service and client](./diagram.png?raw=true)

The Oasis service communicates with the Benternet over ZMQ. The Oasis clients in turn also communicate with the Benternet over ZMQ. No external APIs are used.

## Requests and Responses

The tables below show the requests that can be made to the Oasis service and the responses that can be expected. The first table contains requests that are available to anyone, the second table is only available when logged in. The tables show whether a request has been implemented yet.

Some requests and responses contain variables, a variable will be indicated by square brackets containing the name of the variable and its type, e.g. ``[username:string]``. When the type of a variable is ``list`` it means that this variable contains multiple values separated by a comma ``,``.

The responses show which part of the response the client should subscribe to. This is indicated by curly brackets ``{}``. For example, the following request (checking a client's balance):
```
theoasis>balance?>[username:string]>
```
Has the following response:
```
{theoasis>balance!>[username:string]>}[success:bool]>[balance:int]>[message:string]
```
This means that the client must subscribe to ``theoasis>balance!>[username:string]>``, where the username is the username that they passed to the request.

When a bad request is made (e.g.: wrong username/password, not enough parameters, invalid parameter...) the ``message`` part of the response will be an error message. Depending on the error, this message can range from a generic message such as "bad request", to a more specific message like "invalid bet number". The ``success`` part of the response will also be "false".

Requests available to anyone:
| Description / Request / Response | Implemented |
|---|---|
| Receive an informational message from The Oasis. <br> REQ: theoasis>info?> <br> RES: {theoasis>info!>}[info:string]> | :heavy_check_mark: |
| Receive all the possible requests and responses. <br> REQ: theoasis>help?> <br> RES: {theoasis>help!>}[help:string]> | :heavy_check_mark: |
| Register to The Oasis. A unique username is required. <br> REQ: theoasis>register?>[username:string]>[password:string]> <br> RES: {theoasis>register!>[username:string]>}[success:bool]>[message:string]> | :heavy_check_mark: |
| Login to The Oasis. Must be registered first. <br> REQ: theoasis>login?>[username:string]>[password:string]> <br> RES: {theoasis>login!>[username:string]>}[success:bool]>[message:string]> | :heavy_check_mark: |
| Get more info on the slot machines. <br> REQ: theoasis>info?>slotmachine> <br> RES: {theoasis>info!>slotmachine>} | :heavy_check_mark: |

Requests available only after logging in:
| Description / Request / Response | Implemented |
|---|---|
| Logout of The Oasis. <br> REQ: theoasis>logout?>[username:string]>[password:string]> <br> RES: {theoasis>logout!>[username:string]>}[success:bool]>[message:string]> | :heavy_check_mark: |
| Get your credit balance. <br> REQ: theoasis>balance?>[username:string]> <br> RES: {theoasis>balance!>[username:string]>}[success:bool]>[balance:int]>[message:string]> | :heavy_check_mark: |
| Play a three-reel, single-payline slot machine. The bet is the amount of money you put into the slot machine, which must be lower than your balance and larger than 0. The payline is the three symbols you got. The payout is the amount of money you get back (if this is 0, you lost your bet). <br> REQ: theoasis>slotmachine?>[username:string]>[bet:integer]> <br> RES: {theoasis>slotmachine!>[username:string]>}[success:bool]>[payline:string]>[payout:integer]>[message:string]> | :heavy_check_mark: |
| Play American, double-zero roulette. Bet must be lower than your balance and higher than 0. The betName is the name of the bet you're placing (list can be found at 'theoasis>info?>roulette>'). Some bets require you to specify some numbers, these should be provided in betNumbers. <br> REQ: theoasis>slotmachine?>[username:string]>[bet:integer]>[betName:string]>[betNumbers:list<int>]> <br> RES: {theoasis>slotmachine!>[username:string]>}[success:bool]>[rouletteNumber:string]>[payout:integer]>[message:string]> | :heavy_check_mark: |
