#include "Bank.h"

float Bank::get_interest() {
    return savings_rate;
}

void Bank::invest(float amount) {
    if (amount <= THRESHOLD) {
        cout << "INVESTMENT MUST BE MORE THAN THE THRESHOLD!" << endl;
        return;
    }
    player_savings += amount;
    transaction_history.push_back("INVESTING: +" + to_string(amount));
}

void Bank::update_investment() {
    float return_amount = player_savings * savings_rate/(double)fullreturn;
    player_savings += return_amount;
    transaction_history.push_back("INTEREST EARNED: +" + to_string(return_amount));
}

void Bank::add_forward_deposit(float amount, int term,float rate) {
    forward_deposit_amount.push_back(amount);
    forward_deposit_term.push_back(term);
    forward_deposit_rate.push_back(rate);
    transaction_history.push_back("Forward Deposit: +" + to_string(amount) + " for " + to_string(term) + " months at " + to_string(rate) + "%");
}

float Bank::mature_forward_deposits(int i) 
{
    float matured_amount = forward_deposit_amount[i] * (1 + forward_deposit_rate[i] );
    transaction_history.push_back("Forward Deposit Matured: +" + to_string(matured_amount));
    forward_deposit_amount.erase(forward_deposit_amount.begin()+i);
    forward_deposit_term.erase(forward_deposit_term.begin() + i);
    forward_deposit_rate.erase(forward_deposit_rate.begin() + i);
    return matured_amount;
}

float Bank::get_loan(float required_amount) {
    player_loans += required_amount;
    transaction_history.push_back("Loan Taken: +" + to_string(required_amount));
    return required_amount;

}

void Bank::apply_loan_interest() {
    if (player_loans > 0) {
        float interest_due = player_loans * borrowings_rate /(double)loaninteresttime;
        player_loans += interest_due;
        transaction_history.push_back("Loan Interest Applied: +" + to_string(interest_due));
    }
}

void Bank::repay_loan(float amount) {
    player_loans -= amount;
    credit_score += 10;
    transaction_history.push_back("Loan Repaid: -" + to_string(amount));
}

float Bank::get_player_savings() {
    return player_savings;
}

float Bank::get_player_loans() {
    return player_loans;
}

int Bank::get_credit_score() {
    return credit_score;
}

void Bank::print_transaction_history() {
    cout << "Transaction History:\n";
    for (const auto& transaction : transaction_history) {
        cout << transaction << endl;
    }
}
