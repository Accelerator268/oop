#include "include/figure.h"
#include <iostream>
#include <vector>
#include <memory>
#include <limits>

void print_menu() {
    std::cout << "\nFigure Management System\n";
    std::cout << "1. Add a new figure\n";
    std::cout << "2. Print all figures\n";
    std::cout << "3. Print geometric centers and areas\n";
    std::cout << "4. Calculate total area\n";
    std::cout << "5. Remove figure by index\n";
    std::cout << "6. Clone a figure\n";
    std::cout << "7. Compare two figures\n";
    std::cout << "8. Exit\n";
    std::cout << "Enter your choice: ";
}

int main() {
    std::vector<std::unique_ptr<Figure>> figures;
    
    while (true) {
        print_menu();
        int choice;
        std::cin >> choice;
        
        if (choice == 8) {
            break;
        }
        
        try {
            switch (choice) {
                case 1: {
                    std::cout << "\nEnter figure type (pentagon/5, hexagon/6, octagon/8): ";
                    std::string type;
                    std::cin >> type;
                    
                    auto figure = create_figure(type);
                    std::cin >> *figure;
                    figures.push_back(std::move(figure));
                    std::cout << "Figure added successfully!\n";
                    break;
                }
                
                case 2: {
                    std::cout << "\nAll Figures\n";
                    for (size_t i = 0; i < figures.size(); ++i) {
                        std::cout << "Figure " << i << ":\n";
                        std::cout << *figures[i] << std::endl;
                    }
                    break;
                }
                
                case 3: {
                    std::cout << "\nGeometric Centers and Areas\n";
                    for (size_t i = 0; i < figures.size(); ++i) {
                        std::cout << "Figure " << i << ":\n";
                        print_figure_info(*figures[i]);
                        std::cout << std::endl;
                    }
                    break;
                }
                
                case 4: {
                    double total = calculate_total_area(figures);
                    std::cout << "\nTotal area of all figures: " << total << std::endl;
                    break;
                }
                
                case 5: {
                    std::cout << "\nEnter index of figure to remove (0-" << figures.size() - 1 << "): ";
                    size_t index;
                    std::cin >> index;
                    
                    if (index < figures.size()) {
                        figures.erase(figures.begin() + index);
                        std::cout << "Figure removed successfully!\n";
                    } else {
                        std::cout << "Invalid index!\n";
                    }
                    break;
                }
                
                case 6: {
                    if (figures.empty()) {
                        std::cout << "No figures to clone!\n";
                        break;
                    }
                    
                    std::cout << "\nEnter index of figure to clone (0-" << figures.size() - 1 << "): ";
                    size_t index;
                    std::cin >> index;
                    
                    if (index < figures.size()) {
                        auto cloned = figures[index]->clone();
                        figures.push_back(std::move(cloned));
                        std::cout << "Figure cloned successfully!\n";
                    } else {
                        std::cout << "Invalid index!\n";
                    }
                    break;
                }
                
                case 7: {
                    if (figures.size() < 2) {
                        std::cout << "Need at least 2 figures to compare!\n";
                        break;
                    }
                    
                    std::cout << "\nEnter indices of two figures to compare (0-" << figures.size() - 1 << "): ";
                    size_t idx1, idx2;
                    std::cin >> idx1 >> idx2;
                    
                    if (idx1 < figures.size() && idx2 < figures.size()) {
                        if (*figures[idx1] == *figures[idx2]) {
                            std::cout << "Figures are equal\n";
                        } else {
                            std::cout << "Figures are not equal\n";
                        }
                    } else {
                        std::cout << "Invalid indices!\n";
                    }
                    break;
                }
                
                default:
                    std::cout << "Invalid choice! Please try again.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    
    return 0;
}