#pragma once
#include "gameConst.h"
#include "gameFunction.h"
#define THRESHOLD 100

using namespace std;

class Bank {
public:
    Bank(float savings_rate, float borrowings_rate)
        : savings_rate(savings_rate), borrowings_rate(borrowings_rate), forward_deposit_amount(0), forward_deposit_term(0), forward_deposit_rate(0) {}

    float crr=0.1;

    void apply_loan_interest();

    void repay_loan(float amount);

    void invest(float amount);

    void add_forward_deposit(float amount, int term, float rate);

    float mature_forward_deposits(int i);

    void print_transaction_history();

    void update_investment();

    float get_loan(float required_amount);

    float get_player_savings();

    float get_player_loans();

    float get_interest();

    int get_credit_score();

    float savings_rate;
    float borrowings_rate;
    vector<string> transaction_history;

    float bank_savings = 0;
    float player_savings = 0;
    float player_loans = 0;
    int credit_score = 500;

    vector<float >forward_deposit_amount;
    vector<int> forward_deposit_term;
    vector<float >forward_deposit_rate;
};