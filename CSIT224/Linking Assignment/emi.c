#include <stdio.h>
// #include "my_int_add.c"
// #include "my_int_mult.c"

extern int my_int_add(int a, int b);
extern int my_int_mult(int a, int b);

#define PRINCIPAL 5000000      // Principal amount (Rs. 50 lakhs)
#define INTEREST_RATE 9.0      // Annual interest rate (9% per annum)
#define TENURE 10              // Loan tenure in years
#define MONTHS_IN_YEAR 12.0    // Months in a year

double power(double value, int power) {
    int i = 0;
    double powered_value = 1.00;

    while (i < power) {
        powered_value = powered_value * value;
        // i++;
        i = my_int_add(i,1);
    }

    return powered_value;
}

void calculate_emi_schedule(int p, double i, int n) {
    printf("Payment Schedule:\n");
    printf("Month\tEMI\t\tInterest\tPrincipal\n");

    double remaining_principal = p;
    double emi = p * i / (1 - (1 / power(1 + i, n)));
    for (int month = 1; month <= n; month++) {
        double interest = remaining_principal * i;
        double principal_payment = emi - interest;
        remaining_principal -= principal_payment;

        printf("%-6d  %-12.2lf  %-16.2lf  %.2lf\n", month, emi, interest, principal_payment);
    }
}

int main() {
    int principal_amount = PRINCIPAL;
    int tenure_years = TENURE;
    double monthly_interest_rate = INTEREST_RATE / 100.0 / MONTHS_IN_YEAR;
    // int total_months = tenure_years * MONTHS_IN_YEAR;
    int total_months = my_int_mult(tenure_years, MONTHS_IN_YEAR);

    // Calculate the EMI
    double emi = principal_amount * monthly_interest_rate / (1 - (1 / power(1 + monthly_interest_rate, total_months)));

    // Print the calculated EMI
    printf("Your monthly EMI is: %.2lf\n", emi);

    // Print payment schedule
    calculate_emi_schedule(principal_amount, monthly_interest_rate, total_months);

    return 0;
}