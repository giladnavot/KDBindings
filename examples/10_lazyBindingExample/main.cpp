#include <kdbindings/binding.h>
#include <kdbindings/property.h>
#include <kdbindings/signal.h>

#include <iostream>

using namespace KDBindings;

// A global evaluator that defers updates until evaluateAll() is called
static BindingEvaluator evaluator;

class Stock
{
public:
    // Number of shares and share price
    Property<int> numberOfShares { 100 };
    Property<double> pricePerShare { 20.0 };

    // totalValue is a bound property that depends on the other two properties
    // It will only be recalculated when evaluator.evaluateAll() is called
    const Property<double> totalValue = makeBoundProperty(
        evaluator,
        numberOfShares * pricePerShare
    );
};

int main()
{
    Stock s;

    // Print the initial total value
    std::cout << "Initial total value = " << s.totalValue.get() << std::endl;

    // Connect to the valueChanged signal to observe updates to totalValue
    (void)s.totalValue.valueChanged().connect([](const double &newVal){
        std::cout << "Updated total value = " << newVal << std::endl;
    });

    // Change the price per share; totalValue won't update yet
    s.pricePerShare = 25.0;
    std::cout << "Before evaluateAll, totalValue = " << s.totalValue.get() << std::endl;

    // Force evaluation of all pending bindings
    evaluator.evaluateAll();
    std::cout << "After evaluateAll, totalValue = " << s.totalValue.get() << std::endl << std::endl;

    // Make more changes: both shares and price are updated
    s.numberOfShares = 120;
    s.pricePerShare  = 30.0;

    // Evaluate again to update totalValue
    evaluator.evaluateAll();

    return 0;
}
