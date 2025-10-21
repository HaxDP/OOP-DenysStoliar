using System;
using System.Collections.Generic;
using System.Linq;

namespace Lab3V18
{
    /// <summary>
    /// base class of products
    /// </summary>
    public abstract class FoodItem
    {
        // properties
        public string Name { get; set; }
        public double Calories { get; set; }
        public double Weight { get; set; } // weight in grams

        // base class
        public FoodItem(string name, double calories, double weight)
        {
            Name = name;
            Calories = calories;
            Weight = weight;
        }

        /// <summary>
        /// virtual method for calculating total caloric content
        /// </summary>
        public virtual double CalculateTotalCalories()
        {
            return (Calories * Weight) / 100; // calories on 100g
        }

        /// <summary>
        /// abstract method for obtaining the type of food
        /// </summary>
        public abstract string GetFoodType();

        /// <summary>
        /// virtual method for displaying information
        /// </summary>
        public virtual void DisplayInfo()
        {
            Console.WriteLine($"product: {Name}");
            Console.WriteLine($"type: {GetFoodType()}");
            Console.WriteLine($"caloric content: {Calories} ccal/100g");
            Console.WriteLine($"weight: {Weight}г");
            Console.WriteLine($"total calories: {CalculateTotalCalories():F2} ccal");
            Console.WriteLine("---");
        }

        /// <summary>
        /// Destructor
        /// </summary>
        ~FoodItem()
        {
            Console.WriteLine($"FoodItem '{Name}' is being destroyed");
        }
    }

    /// <summary>
    /// derived class for fruits
    /// </summary>
    public class Fruit : FoodItem
    {
        public bool HasSeeds { get; set; }

        // constructor with a base constructor call
        public Fruit(string name, double calories, double weight, bool hasSeeds) 
            : base(name, calories, weight)
        {
            HasSeeds = hasSeeds;
        }

        /// <summary>
        /// overriding a method to obtain the type of food
        /// </summary>
        public override string GetFoodType()
        {
            return "fruit";
        }

        /// <summary>
        /// overriding the information output method
        /// </summary>
        public override void DisplayInfo()
        {
            base.DisplayInfo();
            Console.WriteLine($"availability of seeds: {(HasSeeds ? "yes" : "no")}");
            Console.WriteLine("---");
        }

        /// <summary>
        /// Destructor
        /// </summary>
        ~Fruit()
        {
            Console.WriteLine($"Fruit '{Name}' is being destroyed");
        }
    }

    /// <summary>
    /// derived class for meat
    /// </summary>
    public class Meat : FoodItem
    {
        public string MeatType { get; set; } // poultry, beef, pork, etc.

        // constructor with a call to the base constructor
        public Meat(string name, double calories, double weight, string meatType) 
            : base(name, calories, weight)
        {
            MeatType = meatType;
        }

        /// <summary>
        /// overriding a method to obtain the type of food
        /// </summary>
        public override string GetFoodType()
        {
            return "meat";
        }

        /// <summary>
        /// redefinition of the calorie calculation method
        /// </summary>
        public override double CalculateTotalCalories()
        {
            // meat may have additional coefficients
            double baseCalories = base.CalculateTotalCalories();
            return baseCalories;
        }

        /// <summary>
        /// overriding the information output method
        /// </summary>
        public override void DisplayInfo()
        {
            base.DisplayInfo();
            Console.WriteLine($"type of meat: {MeatType}");
            Console.WriteLine("---");
        }

        /// <summary>
        /// Destructor
        /// </summary>
        ~Meat()
        {
            Console.WriteLine($"Meat '{Name}' is being destroyed");
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            Console.OutputEncoding = System.Text.Encoding.UTF8;
            Console.WriteLine("=== laboratory work №3: inheritance ===");
            Console.WriteLine("=== option 18: food products ===\n");

            // demonstration of polymorphism: a list of the base type
            List<FoodItem> foodItems = new List<FoodItem>();

            // adding different types of products
            foodItems.Add(new Fruit("apple", 52, 150, true));
            foodItems.Add(new Fruit("banana", 89, 120, false));
            foodItems.Add(new Meat("chicken", 165, 200, "bird"));
            foodItems.Add(new Fruit("orange", 47, 130, true));
            foodItems.Add(new Meat("beef", 250, 180, "red meat"));
            foodItems.Add(new Meat("pork", 242, 220, "pork"));

            // displaying information about all products
            Console.WriteLine("=== information about all products ===");
            foreach (var food in foodItems)
            {
                food.DisplayInfo(); // polymorphism
            }

            // calculating the total calorie content of a dish
            Console.WriteLine("=== calorie calculations ===");
            double totalCalories = foodItems.Sum(f => f.CalculateTotalCalories());
            Console.WriteLine($"total caloric content of all products: {totalCalories:F2} ccal\n");

            // comparison by categories
            var fruits = foodItems.OfType<Fruit>();
            var meats = foodItems.OfType<Meat>();

            double fruitsCalories = fruits.Sum(f => f.CalculateTotalCalories());
            double meatsCalories = meats.Sum(f => f.CalculateTotalCalories());

            Console.WriteLine($"total caloric content of fruits: {fruitsCalories:F2} ccal");
            Console.WriteLine($"total caloric content of meat: {meatsCalories:F2} ccal");
            Console.WriteLine($"quantity of fruits: {fruits.Count()}");
            Console.WriteLine($"number of types of meat: {meats.Count()}\n");

            // comparison of calorie content by category
            if (fruitsCalories > meatsCalories)
            {
                Console.WriteLine("fruits have a higher overall calorie content in the dish");
            }
            else if (meatsCalories > fruitsCalories)
            {
                Console.WriteLine("meat has a higher overall calorie content in the dish");
            }
            else
            {
                Console.WriteLine("the calorie content of fruits and meat is the same");
            }

            // searching for the most caloric product
            var mostCaloric = foodItems.OrderByDescending(f => f.CalculateTotalCalories()).First();
            Console.WriteLine($"\nthe most high-calorie product: {mostCaloric.Name} ({mostCaloric.CalculateTotalCalories():F2} ccal)");

            // demonstration of working with specific types
            Console.WriteLine("\n=== additional information ===");
            foreach (var fruit in fruits)
            {
                Console.WriteLine($"{fruit.Name} - seed: {((Fruit)fruit).HasSeeds}");
            }

            // Clear the list to demonstrate destructors
            foodItems.Clear();
            
            // Force garbage collection to show destructors working
            GC.Collect();
            GC.WaitForPendingFinalizers();

            Console.WriteLine("\nthe program has finished. Press any key...");
            Console.ReadKey();
        }
    }
}