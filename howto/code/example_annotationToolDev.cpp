/**
 *  DEMONSTRATION: Interaction for annotation tools
 * 
 */

#include <fstream>
#include <iostream>
#include <sstream>

#include <libcellml>

int main()
{
    // STEP 1
    // Parse an existing CellML model from a file.

    std::string inFileName = "resources/annotationExample.cellml";
    std::ifstream inFile(inFileName);
    std::stringstream inFileContents;
    inFileContents << inFile.rdbuf();

    auto parser = libcellml::Parser::create();
    auto model = parser->parseModel(inFileContents.str());

    // STEP 2
    // Create an Annotator instance and pass the model to it for processing.
    auto annotator = libcellml::Annotator::create();
    annotator->setModel(model);

    // Retrieve a dictionary of all the id strings and the types of items they represent.
    auto dictionary = annotator->dictionary();
    std::cout << "Existing id strings are:"<<std::endl;
    for(auto const &i : dictionary){
        std::cout << " " << i.first << " = " << annotator->typeAsString(i.second) << std::endl;
    }
    std::cout << std::endl;

    // STEP 3
    // Retrieve items by their id where the item type is known.

    // Retrieve a component with the id of "yellow". We can only do this because 
    // we have prior knowledge that the item with id of "yellow" is actually 
    // a Component.  
    auto component = annotator->component("yellow");

    // The same applies to the other item types below.
    auto variable = annotator->variable("indigo");
    auto reset = annotator->reset("violet");
    auto import = annotator->importSource("orange");
    auto units = annotator->units("green");
    // auto model = annotator->model("red");
    auto unit = annotator->unit("blue");
    auto connection = annotator->connection("beige");
    auto mapVariables = annotator->mapVariables("puce");

    // Some kinds of items are returned by their parent item.  These are:
    //  - componentRef: returns the Component with this id on its encapsulation item.
    auto componentRef = annotator->componentRef("black");

    //  - encapsulation: returns the Model with this id on its encapsulation item.
    auto encapsulation = annotator->encapsulation("brown");

    //  - resetValue: returns the Reset with this id on its reset value.
    auto resetValue = annotator->resetValue("taupe");

    //  - testValue: returns the Reset with this id on its test value.
    auto testValue = annotator->testValue("mauve");
    
    // In this example reset, resetValue and testValue will be the same because the
    // "taupe" reset value and "mauve" test value are in the "violet" reset item.
    std::cout << std::endl;
    // STEP 4
    // Dealing with unique id strings where the item has an unknown type.

    libcellml::AnyItem itemOfUnknownType;

    // Check that the id is unique in the model scope:
    if(annotator->isUnique("green")){
        // Retrieve item from the annotator by their unique id.
        itemOfUnknownType = annotator->item("green");
    }

    // Because these could be any kind of item, they are stored in an AnyItem
    // type.  This is a pair, where the first item is a CellMLElement enum 
    // indicating the item's type, and the second is a std::any cast containing the
    // item itself. 

    // If you don't know the item type, you need to declare all possible types
    // outside of the switch (in this example we're using things we've declared earlier).

    switch(itemOfUnknownType.first) {
        case libcellml::CellMLElement::COMPONENT:
            component = std::any_cast<libcellml::ComponentPtr>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::COMPONENT_REF:
            componentRef = std::any_cast<libcellml::ComponentPtr>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::CONNECTION:
            connection = std::any_cast<libcellml::VariablePair>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::MAP_VARIABLES:
            mapVariables = std::any_cast<libcellml::VariablePair>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::IMPORT:
            import = std::any_cast<libcellml::ImportSourcePtr>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::ENCAPSULATION:
            encapsulation = std::any_cast<libcellml::ModelPtr>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::MATHML:
            break;
        case libcellml::CellMLElement::MODEL:
            // model = std::any_cast<libcellml::ModelPtr>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::RESET:
            reset = std::any_cast<libcellml::ResetPtr>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::RESET_VALUE:
            resetValue = std::any_cast<libcellml::ResetPtr>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::TEST_VALUE:
            testValue = std::any_cast<libcellml::ResetPtr>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::UNIT:
            unit = std::any_cast<libcellml::UnitItem>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::UNITS:
            units = std::any_cast<libcellml::UnitsPtr>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::VARIABLE:
            variable = std::any_cast<libcellml::VariablePtr>(itemOfUnknownType.second);
            break;
        case libcellml::CellMLElement::UNDEFINED:
            break;
    }
    std::cout << std::endl;
    // STEP 5
    // Handling duplicate ID strings.

    // Find any duplicated ID strings inside the model.
    auto duplicatedIds = annotator->duplicateIds();

    std::cout << "There are " << duplicatedIds.size() << " duplicated ids in the model." << std::endl;
    for(auto const &id : duplicatedIds) {
        std::cout << " - " << id << std::endl;
    }

    // Retrieve all items with the given id string. This returns a std::vector
    // of AnyItems which will need to be cast into libcellml items before they
    // can be used.  Note that duplicated ids are not valid CellML, and need 
    // to be fixed before the model can be used.
    auto allItemsWithDuplicateId1 = annotator->items("duplicateId1");

    // A new id string which is automatically generated and unique can be
    // assigned to these items.
    std::cout << "There are " << allItemsWithDuplicateId1.size() << " items with an id of 'duplicateId1'." << std::endl;
    for(auto & i: allItemsWithDuplicateId1) {
        annotator->assignId(i);
    }
    // Now there are no more items with the duplicated id "duplicateId1"
    // remaining in the model.
    allItemsWithDuplicateId1 = annotator->items("duplicateId1");
    std::cout << "There are " << allItemsWithDuplicateId1.size() << " items with an id of 'duplicateId1'." << std::endl;

    // It's straightforward to use a double loop to automatically assign new and unique ids to
    // any duplicated ids in the model.
    for(auto &id: duplicatedIds){
        auto itemsWithThisId = annotator->items(id);
        for(auto &item : itemsWithThisId){
            annotator->assignId(item);
        }
    }

    // Get the list of duplicates again.
    duplicatedIds = annotator->duplicateIds();
    std::cout << "There are " << duplicatedIds.size() << " duplicated ids in the model." << std::endl;
    std::cout << std::endl;
    // STEP 6
    // Automatically assign id strings to anything without them in the model.
    // This can be done by item type (eg: all the components, all the resets, etc)
    // or en masse to everything (excluding MathML) which can have an id attribute.
    
    // Print all component ids to the terminal.
    std::cout << "Before automatic ids are assigned:" << std::endl;
    std::cout << "  Component 1: "<< model->component("component1")->id() << std::endl;
    std::cout << "  Component 2: "<< model->component("component2")->id() << std::endl;
    std::cout << "  Component 3: "<< model->component("component2")->component("component3")->id() << std::endl;
    std::cout << "  Component 4: "<< model->component("component4")->id() << std::endl;

    // Assigns an automatic id string to all Component items which don't already have one.
    annotator->assignIds(libcellml::CellMLElement::COMPONENT);

    std::cout << "After automatic ids are assigned to component items:" << std::endl;
    std::cout << "  Component 1: "<< model->component("component1")->id() << std::endl;
    std::cout << "  Component 2: "<< model->component("component2")->id() << std::endl;
    std::cout << "  Component 3: "<< model->component("component2")->component("component3")->id() << std::endl;
    std::cout << "  Component 4: "<< model->component("component4")->id() << std::endl;

    auto numberOfIds = annotator->dictionary().size();
    std::cout << "Before assigning all automatic ids, there are " << numberOfIds <<" items with an id attribute." << std::endl;

    // Automatically assign ids to everything in the model without one already.
    annotator->assignAllIds();

    // Check the dictionary again.
    numberOfIds = annotator->dictionary().size();
    std::cout << "After assigning all automatic ids, there are " << numberOfIds <<" items with an id attribute." << std::endl;

    // Completely clear all ids in the model.
    annotator->clearAllIds();
    numberOfIds = annotator->dictionary().size();
    std::cout << "After clearing all ids, there are " << numberOfIds <<" items with an id attribute." << std::endl;

    // END
}
