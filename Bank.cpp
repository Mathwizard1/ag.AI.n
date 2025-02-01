#include "Bank.h"

float Bank::get_interest() {
    return savings_rate;
}

void Bank::invest(float amount) {
    if (amount <= THRESHOLD) {
        cout << "INVESTMENT MUST BE MORE THAN THE THRESHOLD!" << endl;
        return;
    }
    bank_savings += amount;
    player_savings += amount;
    transaction_history.push_back("INVESTING: +" + to_string(amount));
}

float Bank::get_return() {
    double elapsed_time = totalticks - last_update_time;
    float return_amount = player_savings * savings_rate * elapsed_time/(double)fullreturn;
    player_savings += return_amount;
    bank_savings += return_amount;
    last_update_time = totalticks;
    transaction_history.push_back("INTEREST EARNED: +" + to_string(return_amount));
    return return_amount;
}

void Bank::add_forward_deposit(float amount, int term, float rate) {
    if (amount <= 0 || term <= 0 || rate <= 0) {
        cout << "Invalid forward deposit parameters." << endl;
        return;
    }
    forward_deposit_amount += amount;
    forward_deposit_term = term;
    forward_deposit_rate = rate;
    bank_savings += amount;
    transaction_history.push_back("Forward Deposit: +" + to_string(amount) + " for " + to_string(term) + " months at " + to_string(rate) + "%");
}

void Bank::mature_forward_deposits() {
    if (forward_deposit_amount > 0 && forward_deposit_term > 0) {
        float matured_amount = forward_deposit_amount * (1 + (forward_deposit_rate / 100.0f) * (forward_deposit_term / 12.0f));
        player_savings += matured_amount;
        bank_savings -= matured_amount;
        transaction_history.push_back("Forward Deposit Matured: +" + to_string(matured_amount));
        forward_deposit_amount = 0;
        forward_deposit_term = 0;
        forward_deposit_rate = 0;
    }
}

float Bank::get_loan(float required_amount) {
    if (required_amount <= 0) {
        cout << "Invalid loan amount." << endl;
        return 0;
    }
    float available_amount = bank_savings * (1 - crr);
    if (required_amount <= available_amount && credit_score >= 500) {
        bank_savings -= required_amount;
        player_loans += required_amount;
        loan_issue_time = time(0);
        transaction_history.push_back("Loan Taken: +" + to_string(required_amount));
        return required_amount;
    }
    else {
        cout << "Loan not approved. Insufficient funds or low credit score." << endl;
        return 0;
    }
}

void Bank::apply_loan_interest() {
    if (player_loans > 0) {
        time_t current_time = time(0);
        double elapsed_time = difftime(current_time, loan_issue_time);
        float interest_due = player_loans * borrowings_rate * (elapsed_time / 31536000.0f);
        player_loans += interest_due;
        bank_savings += interest_due;
        loan_issue_time = current_time;
        transaction_history.push_back("Loan Interest Applied: +" + to_string(interest_due));
    }
}

void Bank::repay_loan(float amount) {
    apply_loan_interest(); // Ensure loan interest is calculated before repayment
    if (amount <= 0 || amount > player_loans) {
        cout << "Invalid repayment amount." << endl;
        return;
    }
    player_loans -= amount;
    bank_savings += amount;
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


/*int main() {
    //  savings rate of 5%  10%
    Bank myBank(0.05, 0.10);

    int choice;
    float amount;

    do {
        cout << "\n=== Bank Menu ===\n";
        cout << "1. Invest Money\n";
        cout << "2. Get Interest Earned\n";
        cout << "3. Take Loan\n";
        cout << "4. Repay Loan\n";
        cout << "5. Add Forward Deposit\n";

        cout << "6. Mature Forward Deposits\n";
        cout << "7. View Player Savings\n";
        cout << "8. View Player Loans\n";
        cout << "9. View Credit Score\n";
        cout << "10. View Transaction History\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Enter investment amount: ";
            cin >> amount;
            myBank.invest(amount);
            break;
        case 2:
            cout << "Interest earned: " << myBank.get_return() << endl;
            break;
        case 3:
            cout << "Enter loan amount: ";
            cin >> amount;
            myBank.get_loan(amount);
            cout << myBank.bank_savings;
            break;
        case 4:
            cout << "Enter repayment amount: ";
            cin >> amount;
            myBank.repay_loan(amount);
            break;
        case 5: {
            float rate;
            int term;
            cout << "Enter deposit amount: ";
            cin >> amount;
            cout << "Enter deposit term (months): ";
            cin >> term;
            cout << "Enter interest rate (%): ";
            cin >> rate;
            myBank.add_forward_deposit(amount, term, rate);
            break;
        }
        case 6:
            myBank.mature_forward_deposits();
            cout << "Matured forward deposits.\n";
            break;
        case 7:
            cout << "Player Savings: " << myBank.get_player_savings() << endl;
            break;
        case 8:
            cout << "Player Loans: " << myBank.get_player_loans() << endl;
            break;
        case 9:
            cout << "Credit Score: " << myBank.get_credit_score() << endl;
            break;
        case 10:
            myBank.print_transaction_history();
            break;
        case 0:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 0);

    return 0;
}
*/