#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

class Product 
{
public:
    int id;
    string name;
    double price;
};

class CartItem 
{
public:
    int productId;
    int quantity;
};

class Cart 
{
public:
    vector<CartItem> items;
};

class IProductCatalog 
{
public:
    virtual Product getProductById(int id) = 0;
    virtual ~IProductCatalog() {}
};

class ICartCalculator 
{
public:
    virtual double calculateTotal(vector<CartItem> items) = 0;
    virtual ~ICartCalculator() {}
};

class ICartStorage 
{
public:
    virtual void save(Cart cart) = 0;
    virtual ~ICartStorage() {}
};

class ProductCatalog : public IProductCatalog
{
private:
    map<int, Product> products;
public:
    ProductCatalog()
    {
        products[1] = {1, "phone", 300.0};
        products[2] = {2, "laptop", 1000.0};
    }
    
    Product getProductById(int id)
    {
        if (products.find(id) != products.end()) 
        {
            return products[id];
        }
        return Product{0, "", 0.0};
    }
};

class CartCalculator : public ICartCalculator
{
private:
    IProductCatalog* catalog;
public:
    CartCalculator(IProductCatalog* cat) : catalog(cat) {}
    
    double calculateTotal(vector<CartItem> items)
    {
        double total = 0.0;
        for (auto& item : items) 
        {
            Product product = catalog->getProductById(item.productId);
            if (product.id != 0) 
            {
                total += product.price * item.quantity;
            }
        }
        return total;
    }
};

class CartStorage : public ICartStorage
{
public:
    void save(Cart cart)
    {
        cout << "cart saved" << endl;
    }
};

class ShoppingCartService
{
private:
    IProductCatalog* catalog;
    ICartCalculator* calculator;
    ICartStorage* storage;
    Cart cart;
public:
    ShoppingCartService(IProductCatalog* cat, ICartCalculator* calc, ICartStorage* stor)
        : catalog(cat), calculator(calc), storage(stor) {}
    
    void addItem(int productId, int quantity) 
    {
        Product product = catalog->getProductById(productId);
        if (product.id == 0) 
        {
            cout << "product not found" << endl;
            return;
        }
        
        bool found = false;
        for (auto& item : cart.items) 
        {
            if (item.productId == productId) 
            {
                item.quantity += quantity;
                found = true;
                break;
            }
        }
        
        if (!found) 
        {
            cart.items.push_back({productId, quantity});
        }
        
        cout << "added " << quantity << " of " << product.name << endl;
    }
    
    void removeItem(int productId)
    {
        for (auto it = cart.items.begin(); it != cart.items.end(); ++it) 
        {
            if (it->productId == productId) 
            {
                cart.items.erase(it);
                cout << "item removed" << endl;
                return;
            }
        }
    }
    
    void calculateAndShowTotal()
    {
        double total = calculator->calculateTotal(cart.items);
        cout << "cart total: " << total << endl;
    }
    
    void saveCart()
    {
        storage->save(cart);
    }
};

class BadShoppingCart
{
private:
    vector<CartItem> items;
    map<int, Product> products;
public:
    BadShoppingCart()
    {
        products[1] = {1, "phone", 300.0};
        products[2] = {2, "laptop", 1000.0};
    }
    
    void addItem(int productId, int quantity)
    {
        if (products.find(productId) == products.end()) 
        {
            cout << "product not found" << endl;
            return;
        }
        
        bool found = false;
        for (auto& item : items) 
        {
            if (item.productId == productId) 
            {
                item.quantity += quantity;
                found = true;
                break;
            }
        }
        
        if (!found) 
        {
            items.push_back({productId, quantity});
        }
        
        cout << "added " << quantity << " of " << products[productId].name << endl;
    }
    
    void removeItem(int productId)
    {
        for (auto it = items.begin(); it != items.end(); ++it) 
        {
            if (it->productId == productId) 
            {
                items.erase(it);
                cout << "item removed" << endl;
                return;
            }
        }
    }
    
    void calculateTotal()
    {
        double total = 0.0;
        for (auto& item : items) 
        {
            if (products.find(item.productId) != products.end()) 
            {
                total += products[item.productId].price * item.quantity;
            }
        }
        cout << "cart total: " << total << endl;
    }
    
    void saveCart()
    {
        cout << "cart saved" << endl;
    }
};

int main()
{
    cout << "=== Bad ShoppingCart ===" << endl;
    BadShoppingCart badCart;
    badCart.addItem(1, 2);
    badCart.calculateTotal();
    badCart.saveCart();
    
    cout << endl << "=== Refactored ShoppingCart ===" << endl;
    ProductCatalog* catalog = new ProductCatalog();
    CartCalculator* calculator = new CartCalculator(catalog);
    CartStorage* storage = new CartStorage();
    ShoppingCartService cartService(catalog, calculator, storage);
    
    cartService.addItem(1, 1);
    cartService.addItem(2, 1);
    cartService.calculateAndShowTotal();
    cartService.saveCart();
    cartService.removeItem(1);
    cartService.calculateAndShowTotal();
    
    delete catalog;
    delete calculator;
    delete storage;
}