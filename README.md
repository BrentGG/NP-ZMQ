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

The [Diagram](#diagram) section shows a diagram of the service, client and Benternet. The [Requests and Responses](#requests-and-responses) section describes all the available and planned requests and responses for the service and each of its games. The [Games](#games) section provides more information on each of the games, as well as an example on how to play the game using the requests and responses.

## Diagram

![Diagram of the Benternet and the Oasis service and client](./media/diagram.png?raw=true)

The Oasis service communicates with the Benternet over ZMQ. The Oasis clients in turn also communicate with the Benternet over ZMQ. No external API is used.

## Requests and Responses

Some requests and responses contain variables, a variable will be indicated by square brackets containing the name of the variable and its type, e.g. ``[username:string]``. When the type of a variable is ``list`` it means that this variable contains multiple values separated by a comma ``,``. Two-dimensional lists are separated by a dash ``-``.

The responses show which part of the response the client should subscribe to in italics. For example, the following request (checking a client's balance):
<pre><code><i>theoasis>balance?></i>[username:string]></code></pre>
Has the following response:
<pre><code><i>theoasis>balance!>[username:string]></i>[success:bool]>[balance:int]>[message:string]</code></pre>
This means that the client must subscribe to ``theoasis>balance!>[username:string]>``, where the username is the username that they passed to the request. This is a suggestions, you can of course subscribe to a more general topic and do the parsing yourself.

When a bad request is made (e.g.: wrong username/password, not enough parameters, invalid parameter...) the ``message`` part of the response will be an error message. Depending on the error, this message can range from a generic message such as "bad request", to a more specific message like "invalid bet number". The ``success`` part of the response will also be "false".

Whenever a cards is communicated, it will be in the format ``list(string, integer)``. This is a list of two items: the suit (spades, hearts, clubs, diamonds) and the number (1 being the ace, and 13 the king). A list of cards is simply a one-dimensional list consisting of a series of string and integer pairs.

<table>
    <thead>
        <tr>
            <th>Description / Request / Response</th>
            <th>Be logged in</th>
            <th>Implemented</th>
        </tr>
    </thead>
    <tbody> <!-- GENERAL -->
        <tr>
            <th colspan=3>General</th>
        </tr>
        <tr>
            <td>
            <b>Receive an informational message from The Oasis.</b> <br> 
            REQ: <code>theoasis>info?></code> <br> 
            RES: <code><i>theoasis>info!></i>[info:string]></code>
            </td>
            <td>:x:</td>
            <td>:heavy_check_mark:</td>
        </tr>
        <tr>
            <td>
            <b>Receive all the possible requests and responses.</b> <br> 
            REQ: <code>theoasis>help?></code> <br> 
            RES: <code><i>theoasis>help!></i>[help:string]></code>
            </td>
            <td>:x:</td>
            <td>:heavy_check_mark:</td>
        </tr>
        <tr>
            <td>
            <b>Register to The Oasis.</b> A unique username is required. <br> 
            REQ: <code>theoasis>register?>[username:string]>[password:string]></code> <br> 
            RES: <code><i>theoasis>register!>[username:string]></i>[success:bool]>[message:string]></code>
            </td>
            <td>:x:</td>
            <td>:heavy_check_mark:</td>
        </tr>
        <tr>
            <td>
            <b>Login to The Oasis.</b> Must be registered first. <br> 
            REQ: <code>theoasis>login?>[username:string]>[password:string]></code> <br> 
            RES: <code><i>theoasis>login!>[username:string]></i>[success:bool]>[message:string]></code>
            </td>
            <td>:x:</td>
            <td>:heavy_check_mark:</td>
        </tr>
        <tr>
            <td>
            <b>Logout of The Oasis.</b> <br> 
            REQ: <code>theoasis>logout?>[username:string]>[password:string]></code> <br> 
            RES: <code><i>theoasis>logout!>[username:string]></i>[success:bool]>[message:string]></code>
            </td>
            <td>:heavy_check_mark:</td>
            <td>:heavy_check_mark:</td>
        </tr>
        <tr>
            <td>
            <b>Get your credit balance.</b> <br> 
            REQ: <code>theoasis>balance?>[username:string]></code> <br> 
            RES: <code><i>theoasis>balance!>[username:string]></i>[success:bool]>[balance:int]>[message:string]></code>
            </td>
            <td>:heavy_check_mark:</td>
            <td>:heavy_check_mark:</td>
        </tr>
    </tbody>
    <tbody> <!-- SLOT MACHINE -->
        <tr>
            <th colspan=3>Slot machine</th>
        </tr>
        <tr>
            <td>
            <b>Get more info on the slot machines.</b> <br> 
            REQ: <code>theoasis>info?>slotmachine></code> <br> 
            RES: <code><i>theoasis>info!>slotmachine></i></code>
            </td>
            <td>:x:</td>
            <td>:heavy_check_mark:</td>
        </tr>
        <tr>
            <td>
            <b>Play the slot machine.</b> <br> 
            REQ: <code>theoasis>slotmachine?>[username:string]>[bet:integer]></code> <br> 
            RES: <code><i>theoasis>slotmachine!>[username:string]></i>[success:bool]>[payline:list(string)]>[payout:integer]>[message:string]></code>
            </td>
            <td>:heavy_check_mark:</td>
            <td>:heavy_check_mark:</td>
        </tr>
    </tbody>
    <tbody> <!-- ROULETTE -->
        <tr>
            <th colspan=3>Roulette</th>
        </tr>
        <tr>
            <td>
            <b>Get more info on the roulette tables.</b> <br> 
            REQ: <code>theoasis>info?>roulette></code> <br> 
            RES: <code><i>theoasis>info!>roulette></i></code>
            </td>
            <td>:x:</td>
            <td>:heavy_check_mark:</td>
        </tr>
        <tr>
            <td>
            <b>Play roulette.</b> <br> 
            REQ: <code>theoasis>roulette?>[username:string]>[bet:integer]>[betName:string]>[betNumbers:list(integer)]></code> <br> 
            RES: <code><i>theoasis>roulette!>[username:string]></i>[success:bool]>[rouletteNumber:string]>[payout:integer]>[message:string]></code>
            </td>
            <td>:heavy_check_mark:</td>
            <td>:heavy_check_mark:</td>
        </tr>
    </tbody>
    <tbody> <!-- BLACKJACK -->
        <tr>
            <th colspan=3>Blackjack</th>
        </tr>
        <tr>
            <td>
            <b>Get more info on Blackjack.</b> <br> 
            REQ: <code>theoasis>info?>blackjack></code> <br> 
            RES: <code><i>theoasis>info!>blackjack></i></code>
            </td>
            <td>:x:</td>
            <td>:x:</td>
        </tr>
        <tr>
            <td>
            <b>Start a Blackjack game by placing a bet.</b> The response contains the dealer's card, and the two player's cards. <br> 
            REQ: <code>theoasis>blackjack?>[username:string]>[bet:integer]></code> <br> 
            RES: <code><i>theoasis>blackjack!>[username:string]></i>[success:bool]>[dealercards:list(string, integer)]>[playercards:list(list(string, integer))]>[message:string]></code>
            </td>
            <td>:heavy_check_mark:</td>
            <td>:x:</td>
        </tr>
        <tr>
            <td>
            <b>Perform a Blackjack action: hit, stand, split, double, insurance.</b> The action will only be performed if applicable. Actions should always be in the order of the player's hands. So if hit is requested, and then stand, hit applies to the first hand, stand to the second. Insurance bet is always half of the main bet. If the game ends after the action, the payout is added to the response. <br> 
            REQ: <code>theoasis>blackjack?>[username:string]>[action:string]></code> <br> 
            RES: <code><i>theoasis>blackjack!>[username:string]></i>[success:bool]>[dealercards:list(string, integer)]>[playercards:list(list(string, integer))]>[message:string]></code> <br> 
            OR <br> 
            RES: <code><i>theoasis>blackjack!>[username:string]></i>[success:bool]>[dealercards:list(string, integer)]>[playercards:list(list(string, integer))]>[payout:integer]>[message:string]></code>
            </td>
            <td>:heavy_check_mark:</td>
            <td>:x:</td>
        </tr>
        <tr>
            <td>
            <b>This response is sent when the deck is shuffled.</b> <br> 
            RES: <code>theoasis>blackjack!>[username:string]>shuffled></code>
            </td>
            <td>:heavy_check_mark:</td>
            <td>:x:</td>
        </tr>
    </tbody>
</table>

## Games

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

Example:

![Example of a Blackjack game](./media/blackjack_time.png?raw=true)
