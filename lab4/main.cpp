#include "include/figure.h"
#include <iostream>
#include <memory>
#include <limits>

using namespace std;

void print_menu() {
    cout << "\nFigure Management System\n";
    cout << "1. Add a new figure\n";
    cout << "2. Print all figures\n";
    cout << "3. Print geometric centers and areas\n";
    cout << "4. Calculate total area\n";
    cout << "5. Remove figure by index\n";
    cout << "6. Clone a figure\n";
    cout << "7. Compare two figures\n";
    cout << "8. Test template array with different types\n";
    cout << "9. Exit\n";
    cout << "Enter your choice: ";
}

void test_template_arrays() {
    cout << "\n=== Testing template arrays with different types ===\n";
    
    //Тестируем Array<shared_ptr<Figure<int>>>
    cout << "\n1. Testing Array<shared_ptr<Figure<int>>>:\n";
    Array<shared_ptr<Figure<int>>> int_figures;
    
    try {
        auto square_int = create_figure<int>("square");
        cout << "Enter vertices for integer square:\n";
        cin >> *square_int;
        int_figures.push_back(square_int);
        cout << "Integer square added successfully!\n";
        
        //Выводим информацию о фигуре
        cout << "Square information:\n";
        print_figure_info(*square_int);
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    //Тестируем Array<Square<int>>
    cout << "\n2. Testing Array<Square<int>>:\n";
    Array<Square<int>> squares_array;
    
    Square<int> sq1;
    cout << "Enter vertices for Square<int>:\n";
    cin >> sq1;
    squares_array.push_back(sq1);
    cout << "Square<int> added to array! Area: " << sq1.area() << endl;
    
    //Тестируем Array<Figure<double>>
    cout << "\n3. Testing Array<shared_ptr<Figure<double>>>:\n";
    Array<shared_ptr<Figure<double>>> double_figures;
    
    auto rect_double = create_figure<double>("rectangle");
    cout << "Enter vertices for double rectangle:\n";
    cin >> *rect_double;
    double_figures.push_back(rect_double);
    cout << "Double rectangle added successfully! Area: " << rect_double->area() << endl;
    
    cout << "\n=== Template array testing completed ===\n";
}

int main() {
    Array<shared_ptr<Figure<double>>> figures;
    
    while (true) {
        print_menu();
        int choice;
        cin >> choice;
        
        if (choice == 9) {
            break;
        }
        
        try {
            switch (choice) {
                case 1: {
                    cout << "\nEnter figure type (square/4, rectangle/rect, trapezoid/trap): ";
                    string type;
                    cin >> type;
                    
                    auto figure = create_figure<double>(type);
                    cout << "Enter vertices:\n";
                    cin >> *figure;
                    figures.push_back(figure);
                    cout << "Figure added successfully!\n";
                    break;
                }
                
                case 2: {
                    cout << "\nAll Figures:\n";
                    for (size_t i = 0; i < figures.size(); ++i) {
                        cout << "Figure " << i << ":\n";
                        figures[i]->print_vertices(cout);
                        cout << endl;
                    }
                    break;
                }
                
                case 3: {
                    cout << "\nGeometric Centers and Areas:\n";
                    for (size_t i = 0; i < figures.size(); ++i) {
                        cout << "Figure " << i << ":\n";
                        print_figure_info(*figures[i]);
                        cout << endl;
                    }
                    break;
                }
                
                case 4: {
                    double total = calculate_total_area(figures);
                    cout << "\nTotal area of all figures: " << total << endl;
                    break;
                }
                
                case 5: {
                    if (figures.empty()) {
                        cout << "No figures to remove!\n";
                        break;
                    }
                    
                    cout << "\nEnter index of figure to remove (0-" << figures.size() - 1 << "): ";
                    size_t index;
                    cin >> index;
                    
                    if (index < figures.size()) {
                        figures.erase(index);
                        cout << "Figure removed successfully!\n";
                    } else {
                        cout << "Invalid index!\n";
                    }
                    break;
                }
                
                case 6: {
                    if (figures.empty()) {
                        cout << "No figures to clone!\n";
                        break;
                    }
                    
                    cout << "\nEnter index of figure to clone (0-" << figures.size() - 1 << "): ";
                    size_t index;
                    cin >> index;
                    
                    if (index < figures.size()) {
                        auto cloned = figures[index]->clone();
                        figures.push_back(cloned);
                        cout << "Figure cloned successfully!\n";
                    } else {
                        cout << "Invalid index!\n";
                    }
                    break;
                }
                
                case 7: {
                    if (figures.size() < 2) {
                        cout << "Need at least 2 figures to compare!\n";
                        break;
                    }
                    
                    cout << "\nEnter indices of two figures to compare (0-" << figures.size() - 1 << "): ";
                    size_t idx1, idx2;
                    cin >> idx1 >> idx2;
                    
                    if (idx1 < figures.size() && idx2 < figures.size()) {
                        if (*figures[idx1] == *figures[idx2]) {
                            cout << "Figures are equal\n";
                        } else {
                            cout << "Figures are not equal\n";
                        }
                    } else {
                        cout << "Invalid indices!\n";
                    }
                    break;
                }
                
                case 8: {
                    test_template_arrays();
                    break;
                }
                
                default:
                    cout << "Invalid choice! Please try again.\n";
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    
    return 0;
}