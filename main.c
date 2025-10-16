#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h" 


//---PRODUCT STRUCTURE---
typedef struct {
    int id;
    char name[50];
    float price;
    int stock;
} Product;


//---CONSTANTS---
#define ADMIN_PASS "admin123"
#define MAX_PASS_LENGHT 50
#define MAX_PRODUCTS 100
#define PRODUCTS_FILE "products.txt"

Product catalog[MAX_PRODUCTS];
int product_count = 0;


//---SAVE AND LOAD PRODUCTS---
void save_products() {
    
    FILE* file;
    if (fopen_s(&file, PRODUCTS_FILE, "w") != 0) {
        printf("\nError: Could not open file %s for saving!\n", PRODUCTS_FILE);
        return;
    }

	//Count of products
    fprintf(file, "%d\n", product_count);

	// Insert products into file
    for (int i = 0; i < product_count; i++) {
        fprintf(file, "%d %s %.2f %d\n",
            catalog[i].id,
            catalog[i].name,
            catalog[i].price,
            catalog[i].stock);
    }

    fclose(file);
    printf("\nCatalog successfully saved to %s.\n", PRODUCTS_FILE);
}



//---LOAD PRODUCTS---
void load_products() {
    FILE* file;
	//Open file for reading
    if (fopen_s(&file, PRODUCTS_FILE, "r") != 0) {
        printf("\nWarning: Product file not found or could not be opened\n");
        product_count = 0;
        return;
    }

	// Read the count of products
    if (fscanf_s(file, "%d\n", &product_count) != 1) {
        printf("\nError reading product count\n");
        fclose(file);
        product_count = 0;
        return;
    }

	// Read each product
    for (int i = 0; i < product_count; i++) {
        if (fscanf_s(file, "%d %s %lf %d\n",
            &catalog[i].id,
            catalog[i].name,
            &catalog[i].price,
            &catalog[i].stock) != 4)
        {
            printf("\nError reading item %d. Data might be corrupted.\n", i + 1);
            product_count = i; 
            fclose(file);
            return;
        }
    }

    fclose(file);
    printf("\nLoaded %d products from %s.\n", product_count, PRODUCTS_FILE);
}


//---ID GENERATOR---
int get_next_product_id() {
    int max_id = 0;
    for (int i = 0; i < product_count; i++) {
        if (catalog[i].id > max_id) {
            max_id = catalog[i].id;
        }
    }
    return max_id + 1;
}


//---ADD PRODUCT---
void add_product() {
    if (product_count >= MAX_PRODUCTS) {
        printf("\nCatalog is full. Cannot add more products.\n");
        return;
	}

	Product new_product;
	char temp_name[50];

	//give product an ID
	new_product.id = get_next_product_id();

	//clear input buffer
	while (getchar() != '\n');

	//Read product name
    printf("\nEnter Product Name (max 49 chars): ");
    if (fgets(temp_name, 50, stdin) == NULL) {
        printf("\nFailed to read product name.\n");
        return;
    }
    // Remove newline and copy (ensures safety against buffer overflow in file saving)
    temp_name[strcspn(temp_name, "\n")] = 0;
    strncpy_s(new_product.name, 50, temp_name, 49);

	// Add Price
    printf("Enter Price: $");
    if (scanf_s("%f", &new_product.price) != 1 || new_product.price <= 0) {
        printf("\nInvalid or negative price entered. Aborting.\n");
      
        while (getchar() != '\n');
        return;
    }

	// Add Stock
    printf("Enter Stock Quantity: ");
    if (scanf_s("%d", &new_product.stock) != 1 || new_product.stock < 0) {
        printf("\nInvalid stock quantity entered. Aborting.\n");
        
        while (getchar() != '\n');
        return;
    }

    // Clear remaining input before next operation
    while (getchar() != '\n');

    // Add to catalog and save
    catalog[product_count] = new_product;
    product_count++;

    // Save the updated catalog
    save_products(); 

    printf("\nProduct '%s' (ID: %d) added successfully.\n Tap 'Enter' to return to menu\n", new_product.name, new_product.id);
}

//---DELETE PRODUCT---
void delete_product() {
    int del_id;
    printf("\nEnter Product ID to remove: ");
    if (scanf_s("%d", &del_id) != 1) {
        printf("\nInvalid input. Please enter a valid Product ID.\n");
        while (getchar() != '\n');
        return;
    }
    // Find product by ID
    int found_index = -1;
    for (int i = 0; i < product_count; i++) {
        if (catalog[i].id == del_id) {
            found_index = i;
            break;
        }
    }
    if (found_index == -1) {
        printf("\nProduct with ID %d not found.\n", del_id);
        return;
    }
    // Shift products to remove the found product
    for (int i = found_index; i < product_count - 1; i++) {
        catalog[i] = catalog[i + 1];
    }
    product_count--;
    // Save the updated catalog
    save_products();
    printf("\nProduct with ID %d removed successfully.\n", del_id);
}

//---VIEW PRODUCTS---
void view_products() {
    if (product_count == 0) {
        printf("\nProduct catalog is empty.\n");
        return;
    }

    printf("\n--- PRODUCT CATALOG ---\n");
    printf("%-5s | %-30s | %-10s | %-5s\n", "ID", "Name", "Price", "Stock");
    printf("----------------------------------------------------------\n");

    for (int i = 0; i < product_count; i++) {
        printf("%-5d | %-30s | $%-9.2f | %-5d\n",
            catalog[i].id,
            catalog[i].name,
            catalog[i].price,
            catalog[i].stock
        );
    }
    printf("----------------------------------------------------------\n");
}


//--- ADMINISTRATOR MENU ---
void admin_menu() {
    int choice;
    int running = 1;

    while (running) {
        printf("\n--- ADMINISTRATOR MENU ---\n");
        printf("1. Add product \n");
        printf("2. Remove product \n");
        printf("3. View all orders \n");
        printf("4. Return to Main Menu\n"); 

        printf("\nEnter your choice: ");
        if (scanf_s("%d", &choice) != 1) {
            printf("\nInvalid input. Please try again.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
        case 1: add_product(); break;
        case 2: delete_product(); break;
        case 3: printf("Selected: View all orders \n"); break;
        case 4: 
            printf("Returning to main menu...\n");
            running = 0;
            break;
        default:
            printf("\nInvalid choice. Please select 1-4.\n");
            break;
        }
        while (getchar() != '\n');
    }
}

//--- CUSTOMER MENU ---
void customer_menu() {
    int choice;
    int running = 1;

    while (running) {
        printf("\n--- CUSTOMER MENU ---\n");
        printf("1. View products\n");
        printf("2. Add product to cart\n");
        printf("3. Checkout\n");
        printf("4. Return to Main Menu\n");

        printf("\nEnter your choice: ");
        if (scanf_s("%d", &choice) != 1) {
            printf("\nInvalid input. Please try again.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
        case 1: view_products(); break;
        case 2: printf("Selected: Add product to cart \n"); break;
        case 3: printf("Selected: Checkout \n"); break;
        case 4:
            printf("Returning to main menu...\n");
            running = 0;
            break;
        default:
            printf("\nInvalid choice. Please select 1-4.\n");
            break;
        }
        while (getchar() != '\n');
    }
}



//---PASSWORD CHECK---
void admin_login() {
    char input_password[MAX_PASS_LENGHT];
    while (_fgetchar() != '\n');

    printf("\nEnter Administrator Password: ");
    if (fgets(input_password, MAX_PASS_LENGHT, stdin) == NULL) {
        printf("\nFailed to read password.\n");
        return;
    }
    // Remove the newline character potentially added by fgets
    input_password[strcspn(input_password, "\n")] = 0;

    // Compare the entered password with the defined constant
    if (strcmp(input_password, ADMIN_PASS) == 0) {
        printf("\n Login successful! Welcome, Admin.\n");
        admin_menu();
    }
    else {
        printf("\n Incorrect Password. Access denied.\n");
    }
}

// --- MAIN ROLE SELECTION MENU ---
void display_main_menu() {
    int choice;
    int running = 1;

    while (running) {
        printf("\n==================================\n");
        printf("         MAIN MENU (Shop)          \n");
        printf("==================================\n");
        printf("1. Login as Administrator\n");
        printf("2. Login as Customer\n");
        printf("3. Exit Program\n");
        printf("----------------------------------\n");
        printf("Enter your choice: ");

        // Input validation
        if (scanf_s("%d", &choice) != 1) {
            printf("\n Invalid input. Please try again.\n");
            // Clear the input buffer (important for scanf)
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
        case 1:
            admin_login();
            break;
        case 2:
            customer_menu();
            break;
        case 3:
            printf("\nExiting program. Goodbye!\n");
            running = 0;
            break;
        default:
            printf("\nInvalid choice. Please select 1, 2, or 3.\n");
            break;
        }
    }
}

// --- PROGRAM ENTRY POINT ---
int main() {
    display_main_menu();
    return 0;
}