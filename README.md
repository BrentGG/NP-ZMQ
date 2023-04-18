# Network Programming - ZMQ Service

## General

This project contains a ZMQ service called The Oasis, which is a virtual casino where clients can play a number of casino games. The client accounts, which includes their username, password, and credit amount, are stored in an SQLite database.

Some general rules of the casino:
- Clients must register once and then log in every time they wish to start a session. New clients start with 1000 credits. 
- Clients are automatically logged out after they've been inactive for at least 15 minutes.
- If a player is logged out due to inactivity while in a game, they lose any bet.
- Logging out manually is not possible while in a game.
- Bets are always paid up front, which means once the bet has been made, the credits go out of the player's account. The payouts in the tables below are always multiplied by the bet.

A client for the service will be made so players can easily explore the casino.

The [Diagram](#diagram) section shows a diagram of the service, client and Benternet. The [Requests and Responses](#requests-and-responses) section describes all the available and planned requests and responses for the service and each of its games, as well as information on how to use them.

## Diagram

![Diagram of the Benternet and the Oasis service and client](./media/diagram.png?raw=true)

The Oasis service communicates with the Benternet over ZMQ. The Oasis clients in turn also communicate with the Benternet over ZMQ. No external API is used.

## Requests and Responses

The [Format](#format) section describes the format that is used for the requests and responses. The [General](#general-1) section describes all the general requests that can be made together with their responses. The sections after that describe the request and responses per game that can be played in the casino. Some requests require the client to be logged in. Some responses are sent without a request being set first. Each table also shows whether a request has been implemented yet. Every game section also contains an example of the requests and responses in a time-diagram fashion. Below is a list of all the available games:

- [Slot machine](#slot-machine)
- [Roulette](#roulette)
- [Blackjack](#blackjack)

### Format

Some requests and responses contain variables, a variable will be indicated by square brackets containing the name of the variable and its type, e.g. ``[username:string]``. When the type of a variable is ``list`` it means that this variable contains multiple values separated by a comma ``,``. Two-dimensional lists are separated by a dash ``-``.

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

Whenever a cards is communicated, it will be in the format ``list(string, integer)``. This is a list of two items: the suit (spades, hearts, clubs, diamonds) and the number (1 being the ace, and 13 the king). A list of cards is simply a one-dimensional list consisting of a series of string and integer pairs.

### General

General requests that are not specific to a game.

Requests available to anyone:
| Description / Request / Response | Implemented |
|---|---|
| **Receive an informational message from The Oasis.** <br> REQ: theoasis>info?> <br> RES: {theoasis>info!>}[info:string]> | :heavy_check_mark: |
| **Receive all the possible requests and responses.** <br> REQ: theoasis>help?> <br> RES: {theoasis>help!>}[help:string]> | :heavy_check_mark: |
| **Register to The Oasis.** A unique username is required. <br> REQ: theoasis>register?>[username:string]>[password:string]> <br> RES: {theoasis>register!>[username:string]>}[success:bool]>[message:string]> | :heavy_check_mark: |
| **Login to The Oasis.** Must be registered first. <br> REQ: theoasis>login?>[username:string]>[password:string]> <br> RES: {theoasis>login!>[username:string]>}[success:bool]>[message:string]> | :heavy_check_mark: |

Requests available only after logging in:
| Description / Request / Response | Implemented |
|---|---|
| **Logout of The Oasis.** <br> REQ: theoasis>logout?>[username:string]>[password:string]> <br> RES: {theoasis>logout!>[username:string]>}[success:bool]>[message:string]> | :heavy_check_mark: |
| **Get your credit balance.** <br> REQ: theoasis>balance?>[username:string]> <br> RES: {theoasis>balance!>[username:string]>}[success:bool]>[balance:int]>[message:string]> | :heavy_check_mark: |

### Slot Machine

The slot machine is a three-reel, single-payline slot machine. The payline is the three symbols that show up on the slot machine, these determine the size of the payout. All the possible symbols are: bar, cherry, plum, watermelon, orange, lemon, empty.

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
| **Get more info on the slot machines.** <br> REQ: theoasis>info?>slotmachine> <br> RES: {theoasis>info!>slotmachine>} | :heavy_check_mark: |

Requests available only after logging in:
| Description / Request / Response | Implemented |
|---|---|
| **Play the slot machine.** <br> REQ: theoasis>slotmachine?>[username:string]>[bet:integer]> <br> RES: {theoasis>slotmachine!>[username:string]>}[success:bool]>[payline:list(string)]>[payout:integer]>[message:string]> | :heavy_check_mark: |

Example:

![Example of the slot machine](./media/slotmachine_time.png?raw=true)

### Roulette
The roulette tables are American, double-zero roulette tables. The table looks like this:

![American, double-zero roulette table](./media/roulette_table.PNG?raw=true)

The ``betName`` should be one of the bet names in the table below, which also show the payout (which is multiplied by the bet) and the meaning of the bet name. If the description of the bet name starts with the word 'any', the client should provide a number or list of numbers (depending on the bet name), into the ``betNumber`` variable. The ``rouletteNumber`` in the response is the number that the roulette landed on (0-36 and 00).

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
| **Get more info on the roulette tables.** <br> REQ: theoasis>info?>roulette> <br> RES: {theoasis>info!>roulette>} | :heavy_check_mark: |

Requests available only after logging in:
| Description / Request / Response | Implemented |
|---|---|
| **Play roulette.** <br> REQ: theoasis>roulette?>[username:string]>[bet:integer]>[betName:string]>[betNumbers:list(integer)]> <br> RES: {theoasis>roulette!>[username:string]>}[success:bool]>[rouletteNumber:string]>[payout:integer]>[message:string]> | :heavy_check_mark: |

Example:

![Example of the roulette table](./media/roulette_time.png?raw=true)

### Blackjack

A standard game of blackjack consisting of a single deck.

The payouts are as follows:
| Event | Payout |
|---|---|
| Blackjack | 2.5 |
| Win | 2 |
| Push | 1 |
| Loss | 0 |

Requests available to anyone:
| Description / Request / Response | Implemented |
|---|---|
| **Get more info on Blackjack.** <br> REQ: theoasis>info?>blackjack> <br> RES: {theoasis>info!>blackjack>} | :x: |

Requests available only after logging in:
| Description / Request / Response | Implemented |
|---|---|
| **Start a Blackjack game by placing a bet.** The response contains the dealer's card, and the two player's cards. <br> REQ: theoasis>blackjack?>[username:string]>[bet:integer]> <br> RES: {theoasis>blackjack!>[username:string]>}[success:bool]>[dealercards:list(string, integer)]>[playercards:list(list(string, integer))]>[message:string]>v | :x: |
| **Perform a Blackjack action: hit, stand, split, double, insurance.** The action will only be performed if applicable. Actions should always be in the order of the player's hands. So if hit is requested, and then stand, hit applies to the first hand, stand to the second. Insurance bet is always half of the main bet. If the game ends after the action, the payout is added to the response. <br> REQ: theoasis>blackjack?>[username:string]>[action:string]> <br> RES: {theoasis>blackjack!>[username:string]>}[success:bool]>[dealercards:list(string, integer)]>[playercards:list(list(string, integer))]>[message:string]> <br> OR <br> RES: {theoasis>blackjack!>[username:string]>}[success:bool]>[dealercards:list(string, integer)]>[playercards:list(list(string, integer))]>[payout:integer]>[message:string]> | :x: |

Responses that are sent without a request:
| Description / Request / Response | Implemented |
|---|---|
| **This response is sent when the deck is shuffled.** <br> RES: theoasis>blackjack!>[username:string]>shuffled> | :x: |

Example:

![Example of a Blackjack game](./media/blackjack_time.png?raw=true)
