#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <vector>
using namespace std;

class Card {
	int number;
	string pattern;
	// To display card number to player
	static const string numberToString[14];

public:
	Card(int _number, string _pattern) {
		number = _number;
		pattern = _pattern;
	}
	int getNumber() {return number;}
	void setNumber(const int _num) {number = _num;}
	string getPattern() {return pattern;}
	void setPattern(const string _pat) {pattern = _pat;}
	string strCardNumber() {return numberToString[number];}
};

const string Card::numberToString[14] = {"0", "Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jessy", "Queen", "King"};


//create decks of cards
class Deck {
	queue<Card> card;
	int numDeck;

	void initialize() {
		// Card initCard[52 * numDeck];
		vector<Card> initCard;
		static const string patterns[] = {"spade", "clover", "heart", "diamond"};
		// fill decks of cards
		for (int i = 0; i < 52 * numDeck; i++) {
			Card *temp = new Card(i % 13 + 1, patterns[i % 4]);
			initCard.push_back(*temp);
			delete temp;
		}

		//shuffle
		srand((unsigned int)time(NULL));
		int dup[52 * numDeck];  // check duplicates of indexes when shuffling.
		for (int i = 0; i < 52 * numDeck; i++) {
			int idx;
			bool dupFlag;
			do {
				idx = rand() % (52 * numDeck);
				dupFlag = false;
				for (int j = 0; j < i; j++) {
					if (dup[j] == idx) {
						dupFlag = true;
						break;
					}
				}
			} while(dupFlag);

			card.push(initCard[idx]);
			dup[i] = idx;
		}
	}

public:
	Deck(int _numDeck = 1) {
		numDeck = _numDeck;
		initialize();
	}

	Card pop() {
		Card pop = card.front();
		card.pop();
		//if card is all gone, shuffle another deck.
		if (card.empty()) {
			initialize();
		}
		return pop;
	}
};

class Player {
	bool isDealer;
	int chip;
	bool isStanding;
	vector<Card> hand;

public:
	Player(bool _dealer, int _chip) {
		isDealer = _dealer;
		chip = _chip;
		isStanding = false;
	}

	// action of player
	void stand() {isStanding = true;}
	void hit(Card _card) {hand.push_back(_card);}
	void changeAmountOfChip(int _chip) {
		chip += _chip;
		if (chip < 0) {
			chip = 0;
		}
	}

	// get player status
	bool isStand() {return isStanding;}
	int getChip() {return chip;}
	string getPlayerHand(bool hide = false) {
		string ret = "";
		vector<Card>::iterator it = hand.begin();
		if (isDealer && hide) {
			// dealer hides one card.
			ret = "[(Not revealed)]";
			++it;
		}
		for (; it != hand.end(); ++it) {
			Card temp = *it;
			ret += "[" + temp.strCardNumber() + " - " + temp.getPattern() + "]";
		}
		return ret;
	}
	bool isBlackjack() {
		// one of two cards should be Ace.
		if (hand[0].getNumber() != 1 && hand[1].getNumber() != 1) {
			return false;
		// one of two cards should have value of 10.
		} else if (hand[0].getNumber() < 10 && hand[1].getNumber() < 10) {
			return false;
		}
		return true;
	}
	bool isBust() {
		// if counting is over 21 -> bust
		if (count() > 21) {
			return true;
		}
		return false;
	}
	// to decide for dealer to get new card.
	bool isBelowSeventeen() {
		if (count() < 17) {
			return true;
		}
		return false;
	}
	int count() {
		int sum = 0;
		int softAce = 0;
		for (vector<Card>::iterator it = hand.begin(); it < hand.end(); ++it) {
			Card temp = *it;
			int normalizedNumber = temp.getNumber();
			// normalize number for blackjack rule
			if (normalizedNumber > 10) {
				normalizedNumber = 10;
			} else if (normalizedNumber == 1) {
				normalizedNumber = 11;
				softAce += 1;
			}
			sum += normalizedNumber;
		}
		// normalize again
		if (sum > 21) {
			// if has soft Ace, don't make it bust
			while (softAce && sum > 21) {
				sum -= 10;
				softAce -= 1;
			}
		}
		return sum;
	}
};


int main() {
	cout << "Welcome to the Casino World." << "\n";
	bool newGame = true;
	int usersChip = 100;
	while (newGame) {
		Player *dealer = new Player(true, -1);
		Player *player = new Player(false, usersChip);
		Deck *deck = new Deck(3);
		int betChip = 0;

		// game start
		int userBetting = 0;
		// betting
		cout << "You have " << player->getChip() << " chips." << endl;
		cout << "how many chips would you bet? (Bet at least 1 chip) :";
		cin >> userBetting;
		if (userBetting <= 0) {
			cout << "You have to bet at least 1 chip.\nQuitting game..." << endl;
			break;
		} else if (userBetting > player->getChip()) {
			userBetting = player->getChip();
			cout << "You have only " << userBetting << " chips." << endl;
		}
		betChip += userBetting;
		cout << "You bet " << userBetting << " chips" << endl;
		player->changeAmountOfChip((-1) * userBetting);

		// pop cards
		player->hit(deck->pop());
		player->hit(deck->pop());
		dealer->hit(deck->pop());
		dealer->hit(deck->pop());

		while (true) {
			cout << "Your hand is " << player->getPlayerHand() << endl;
			cout << "Dealer's hand is " << dealer->getPlayerHand(true) << endl;

			// blackjack cases.
			if (player->isBlackjack() && dealer->isBlackjack()) {
				player->changeAmountOfChip(betChip);
				cout << "Push. Both of you and dealer got Blackjack!" << endl;
				break;
			} else if (player->isBlackjack()) {
				// if blackjack, earn 1.5 chips of betting amount.
				player->changeAmountOfChip(betChip * 2.5);
				cout << "You've got Blackjack! You earned " << betChip * 1.5 << " chip." << endl;
				break;
			} else if (dealer->isBlackjack()) {
				cout << "Dealer got Blackjack! You lost!\nYour hand: " << player->getPlayerHand() << "\nDealer's hand: " << dealer->getPlayerHand() << endl;
				break;
			}

			// while player didn't stand yet...
			if (!player->isStand()) {
				cout << "Do you want one more card? (y/n) : ";
				char moreCard;
				cin >> moreCard;
				if (moreCard == 'y') {
					player->hit(deck->pop());
					if (player->isBust()) {
						cout << "You go bust! Your hand is " << player->getPlayerHand() << endl;
						break;
					}
				} else {
					player->stand();
					// player stands, dealer has higher number -> dealer wins.
					if (player->count() < dealer->count()) {
						cout << "You lost!\nYour hand: " << player->getPlayerHand() << "\nDealer's hand: " << dealer->getPlayerHand() << endl;
						break;
					}
				}
			}

			// if dealer's hand is less than 17
			if (dealer->isBelowSeventeen()) {
				// hit
				dealer->hit(deck->pop());
				// if goes bust, dealer loses.
				if (dealer->isBust()) {
					player->changeAmountOfChip(betChip * 2);
					cout << "Dealer's hand : " << dealer->getPlayerHand() << endl;
					cout << "You win! Dealer goes bust! You earned " << betChip << " chip." << endl;
					break;
				}
			}

			if (player->isStand()) {
				if (player->count() < dealer->count()) {
					// player stands, dealer has higher number -> dealer wins.
					cout << "You lost!\nYour hand: " << player->getPlayerHand() << "\nDealer's hand: " << dealer->getPlayerHand() << endl;
					break;
				}
				if (!dealer->isBelowSeventeen()) {
					// player stands, dealer is over 17.. -> end of game.
					if (player->count() > dealer->count()) {
						player->changeAmountOfChip(betChip * 2);
						cout << "You win!\nYour hand: " << player->getPlayerHand() << "\nDealer's hand: " << dealer->getPlayerHand() << endl;
					} else {
						//push
						player->changeAmountOfChip(betChip);
						cout << "Push this game.\nYour hand: " << player->getPlayerHand() << "\nDealer's hand: " << dealer->getPlayerHand() << endl;
					}
					break;
				}
			}
		}
		// clear up
		usersChip = player->getChip();
		delete deck;
		delete player;
		delete dealer;

		if (usersChip == 0) {
			cout << "You are bankrupt...\nQuitting game..." << endl;
			break;
		}
		char userInput;
		cout << "Do you want to play a new game? (y/n) ";
		cin >> userInput;
		if (userInput == 'n') {
			newGame = false;
		}
	}

	return 0;
}
