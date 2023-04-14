# Network Programming - ZMQ Service

## General

This project contains a ZMQ service called The Oasis, which is a virtual casino where clients can play a number of casino games. Clients must register once and then log in every time they wish to start a session. New clients start with 1000 credits. The client accounts, which includes their username, password, and credit amount, are stored in an SQLite database. Clients are automatically logged out after they've been inactive for at least 15 minutes.

The 'Diagram' section shows a diagram of the service, client and Benternet. The 'Requests and Responses' sections describes all the available and planned requests and responses for the service and each of its games, as well as information on how to use them.

## Diagram

![Diagram of the Benternet and the Oasis service and client](./diagram.png?raw=true)

The Oasis and Trivia Time services communicate with the Benternet over ZMQ. The Oasis and Trivia Time clients in turn also communicate with the Benternet over ZMQ. Trivia Time uses the [Open Trivia Database](https://opentdb.com/) API for its trivia questions and communicates with it using HTTP.

## Requests and Responses

The 'Format' sections describes the format that is used for the requests and responses. The 'General' sections describes all the general requests that can be made together with their responses. The sections after that describe the request and responses per game that can be played in the casino. Some requests require the client to be logged in. Each table also shows whether a request has been implemented yet.

### Format

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

### General

Requests available to anyone:
| Description / Request / Response | Implemented |
|---|---|
| Receive an informational message from The Oasis. <br> REQ: theoasis>info?> <br> RES: {theoasis>info!>}[info:string]> | :heavy_check_mark: |
| Receive all the possible requests and responses. <br> REQ: theoasis>help?> <br> RES: {theoasis>help!>}[help:string]> | :heavy_check_mark: |
| Register to The Oasis. A unique username is required. <br> REQ: theoasis>register?>[username:string]>[password:string]> <br> RES: {theoasis>register!>[username:string]>}[success:bool]>[message:string]> | :heavy_check_mark: |
| Login to The Oasis. Must be registered first. <br> REQ: theoasis>login?>[username:string]>[password:string]> <br> RES: {theoasis>login!>[username:string]>}[success:bool]>[message:string]> | :heavy_check_mark: |

Requests available only after logging in:
| Description / Request / Response | Implemented |
|---|---|
| Logout of The Oasis. <br> REQ: theoasis>logout?>[username:string]>[password:string]> <br> RES: {theoasis>logout!>[username:string]>}[success:bool]>[message:string]> | :heavy_check_mark: |
| Get your credit balance. <br> REQ: theoasis>balance?>[username:string]> <br> RES: {theoasis>balance!>[username:string]>}[success:bool]>[balance:int]>[message:string]> | :heavy_check_mark: |

### Slot Machine

The slot machine is a three-reel, single-payline slot machine. The client must 'insert' a bet into the slot machine. This means that if the payout is 0, the client lost their bet. If the payout is larger than 0, the client earned their bet back, plus a certain amount depending on the payline. The payline is the three symbols that show up on the slot machine, these determine the size of the payout. All the possible symbols are: bar, cherry, plum, watermelon, orange, lemon, empty.

The table below contains the paylines and their payouts, which is multiplied by the bet.
| Payline | Payout |
| --- | --- |
| Three Bar                | 5000
| Three Cherry             | 1000
| Three Plum               | 200
| Three Watermelon         | 100
| Three Orange             | 50
| Thee Lemon               | 25
| Two Cherry (in a row)    | 10
| One Cherry               | 2
| Any Other                | 0

Requests available to anyone:
| Description / Request / Response | Implemented |
|---|---|
| Get more info on the slot machines. <br> REQ: theoasis>info?>slotmachine> <br> RES: {theoasis>info!>slotmachine>} | :heavy_check_mark: |

Requests available only after logging in:
| Description / Request / Response | Implemented |
|---|---|
| Play the slot machine. <br> REQ: theoasis>slotmachine?>[username:string]>[bet:integer]> <br> RES: {theoasis>slotmachine!>[username:string]>}[success:bool]>[payline:list<string>]>[payout:integer]>[message:string]> | :heavy_check_mark: |

### Roulette
The roulette tables are American, double-zero roulette tables. The table looks like this:

![American, double-zero roulette table](./roulette_table.PNG?raw=true)

The client must place a bet onto the table. This means that if the payout is 0, the client lost their bet. The ``betName`` should be one of the bet names in the table below, which also show the payout (which is multiplied by the bet) and the meaning of the bet name. If the description of the bet name starts with the word 'any', the client should provide a number or list of numbers (depending on the bet name), into the ``betNumber`` variable. The ``rouletteNumber`` in the response is the number that the roulette landed on (0-36 and 00).

| Bet Name                 | Payout   | Description |
| --- | --- | --- |
| zero                     | 36       | 0
| double zero              | 36       | 00
| straight up              | 36       | any number (1-36)
| row                      | 18       | 0, 00
| split                    | 18       | any two adjacent numbers (1-36), e.g.: 1,2 or 11,14
| street                   | 12       | any three numbers horizontal (1-36), e.g.: 1,2,3 or 4,5,6 or 34,35,36
| corner                   | 9        | any four adjoining numbers in a block, e.g.: 4,5,7,8
| basket                   | 7        | 0, 00, 1, 2, 3
| double street            | 6        | any two streets, e.g.: 1,2,3,34,35,36
| first column             | 3        | 1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34
| second column            | 3        | 2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35
| third column             | 3        | 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36
| first dozen              | 3        | 1 to 12
| second dozen             | 3        | 13 to 24
| third dozen              | 3        | 25 to 36
| odd                      | 2        | odd numbers
| even                     | 2        | even numbers
| red                      | 2        | red numbers
| black                    | 2        | black numbers
| one to eighteen          | 2        | 1 to 18
| nineteen to thirty-six   | 2        | 19 to 36

Requests available to anyone:
| Description / Request / Response | Implemented |
|---|---|
| Get more info on the roulette tables. <br> REQ: theoasis>info?>roulette> <br> RES: {theoasis>info!>roulette>} | :heavy_check_mark: |

Requests available only after logging in:
| Description / Request / Response | Implemented |
|---|---|
| Play roulette. <br> REQ: theoasis>roulette?>[username:string]>[bet:integer]>[betName:string]>[betNumbers:list<int>]> <br> RES: {theoasis>roulette!>[username:string]>}[success:bool]>[rouletteNumber:string]>[payout:integer]>[message:string]> | :heavy_check_mark: |
