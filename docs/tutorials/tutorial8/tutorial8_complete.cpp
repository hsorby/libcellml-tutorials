/**
 *      TUTORIAL 8: IMPORTS and FLATTENING
 *
 *
 */

#include <iostream>
#include <libcellml>
#include <regex>

#include "../utilities/tutorial_utilities.h"

int main()
{
    //  0.a Create a new model instance representing the combined model and name it.
    auto model = libcellml::Model::create("Tutorial8_HHModel");
    auto validator = libcellml::Validator::create();

    // Create a new controller model, containing the controller component and a parameters component.
    //    This will be written separately to the main model so that its values can be changed easily.
    auto controllerModel = libcellml::Model::create("Tutorial8_parameters");
    auto parameters = libcellml::Component::create("parameters");
    controllerModel->addComponent(parameters);

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 1: Import the membrane component          " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  In order to use the import functionality we need to know three things:
    //      - the import destination (the component/unit to which the imported item will be assigned)
    //      - the file we're importing it from (the url to the model containing the item to be imported)
    //      - which item within the file should be imported (the name of the component/units inside the import model file)
    //  We'll address these now.

    //  1.a As previously, create a component to represent the membrane in the HH model,
    //      and add it to the model.  This deals with the first point above: the import destination
    auto membrane = libcellml::Component::create("membrane");
    model->addComponent(membrane);

    //  1.b Next we need to create an ImportSource item and use its setUrl() function to specify the
    //      name (without the path) of the file contianing the model to be imported.
    auto membraneImporter = libcellml::ImportSource::create();
    membraneImporter->setUrl("tutorial8_MembraneModel.cellml");

    //  1.c Thirdly we need to link our import source to the import destination using the
    //      Component::setImportSource() function on the membrane component, and to use the
    //      Component::setImportReference() function to specify the name of the component inside
    //      the import model to retrieve.
    membrane->setImportSource(membraneImporter);
    membrane->setImportReference("membrane");

    //  1.d At this stage, our model has only one component in it, and even though we've specified the
    //      imports completely, we still need to resolve the imports and flatten the model before the
    //      imported items will be instantiated here.
    //      Use the Model::hasUnresolvedImports() function to show that the imports have not been resolved yet.
    if (model->hasUnresolvedImports()) {
        std::cout << "Imports are UNRESOLVED" << std::endl;
    } else {
        std::cout << "Imports are found" << std::endl;
    }

    //  1.e Print the model to the terminal to show that it contains only one empty component at this stage.
    printModelToTerminal(model, false);

    //  1.f Use the Model::resolveImports() function of the model to (erm) resolve the imports.  This takes an
    //      argument of a string representing the full absolute path to the directory in which the import
    //      file specified in 1.b is stored, and must end with a slash.
    model->resolveImports("");

    //  1.g Call the Model::hasUnreolvedImports() function again and verify that they are now resolved.
    assert(model->hasUnresolvedImports() == false);
    std::cout << "Imports are RESOLVED" << std::endl;

    //  1.h Call the Model::flatten() function.  This will recursively search through all of the imported items
    //      in the model, and create local instances of them here.
    //      Note that:
    //          - if you call the flatten() function without first resolving the imports, nothing will change.
    //          - flattening a model fundamentally changes it ... and cannot be undone.
    model->flatten();
    membrane = model->component("membrane");

    //  1.i Print the flattened model to the terminal and verify that it now contains:
    //      - 4 units
    //      - 1 component with
    //          - 8 variables
    //          - a mathml block
    //  from the imported file.
    printModelToTerminal(model, false);

    //  1.j Because we want to be able to initialise the variables in this component, we need to
    //      add the corresponding variables into the parameters component.  These will be variables:
    //      - Cm [micro Farads per square centimetre]
    //      - V [millivolts].
    //      Add two variables to the parameters component, with an interface type of "public".
    //      You will need to keep track of the units which you add to this component as you go, and
    //      add in any new ones.  In this case, both are new and will need to be created and added
    //      to the controller model as normal.
    {
        auto Cm = libcellml::Variable::create("Cm");
        parameters->addVariable(Cm);
        parameters->variable("Cm")->setUnits("microF_per_cm2");
        parameters->variable("Cm")->setInterfaceType("public_and_private");

        auto microF_per_cm2 = libcellml::Units::create("microF_per_cm2");
        microF_per_cm2->addUnit("farad", "micro");
        microF_per_cm2->addUnit("metre", "centi", -2);
        controllerModel->addUnits(microF_per_cm2);

        auto V = libcellml::Variable::create("V");
        parameters->addVariable(V);
        parameters->variable("V")->setUnits("mV");
        parameters->variable("V")->setInterfaceType("public_and_private");

        auto mV = libcellml::Units::create("mV");
        mV->addUnit("volt", "milli");
        controllerModel->addUnits(mV);
    }

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 2: Import the sodium channel component    " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  2.a Create a component representing the sodium channel.  This will be encapsulated inside the membrane
    //      component, so add it there instead of adding it to the model.
    auto sodiumChannel = libcellml::Component::create("sodium_channel");
    membrane->addComponent(sodiumChannel);

    //  2.b Create an importer for the sodium channel, and point it to the file you created in Tutorial 7.
    //      Note that you will need to make sure it exists in the same path as the earlier files.
    //      If you did not complete Tutorial 7 you can use the tutorial7_SodiumChannelModel.cellml
    //      file in the resources folder.
    auto sodiumImporter = libcellml::ImportSource::create();
    sodiumImporter->setUrl("tutorial7_SodiumChannelModel.cellml");

    //  2.c Link the sodium channel component to the importer and set the reference to import.
    //      In the file provided this is named "sodiumChannel".  You will need to change this to
    //      whatever you named the component in Tutorial 7.
    sodiumChannel->setImportSource(sodiumImporter);
    sodiumChannel->setImportReference("sodiumChannel");

    //  2.d Create dummy variables in the sodium channel for voltage, time, and current.
    //      Dummy variables are those which already exist in the component to be imported, but must be
    //      added here manually so that we can create the connections between equivalent variables before
    //      flattening the model.
    //      Because these are dummy variables will be overwritten, you do not need to specify units or interfaces.
    {
        auto V = libcellml::Variable::create("V");
        sodiumChannel->addVariable(V);
        auto t = libcellml::Variable::create("t");
        sodiumChannel->addVariable(t);
        auto i_Na = libcellml::Variable::create("i_Na");
        sodiumChannel->addVariable(i_Na);
    }
    //  2.e Create dummy variables for m and h gate status here so that their initial values can be passed to the
    //      gate components, which are child components of the sodium channel.  Even though this component doesn't
    //      use them, the gate components cannot connect directly to the parameters component at the top level,
    //      so these intermediate variables are needed.
    //      Add variables for h, m, g_Na, and E_Na to the sodium channel component.
    //      Because these are dummy variables will be overwritten, you do not need to specify units or interfaces.
    {
        auto h = libcellml::Variable::create("h");
        sodiumChannel->addVariable(h);
        auto m = libcellml::Variable::create("m");
        sodiumChannel->addVariable(m);
        auto g_Na = libcellml::Variable::create("g_Na");
        sodiumChannel->addVariable(g_Na);
        auto E_Na = libcellml::Variable::create("E_Na");
        sodiumChannel->addVariable(E_Na);
    }

    //  2.f Because any values for initialisation must also be passed through the membrane component, we have to
    //      add intermediate variables for h, m, g_Na, and E_Na there too.
    //      Because these are concrete variables (ie: they will not be overwritten by an import), you will need
    //      to specify units and interfaces.  Note that because these variables will need to connect to child
    //      components as well as sibling components, they should have the interface type "public_and_private".
    {
        auto h = libcellml::Variable::create("h");
        membrane->addVariable(h);
        h->setInterfaceType("public_and_private");
        h->setUnits("dimensionless");

        auto m = libcellml::Variable::create("m");
        membrane->addVariable(m);
        m->setInterfaceType("public_and_private");
        m->setUnits("dimensionless");

        auto g_Na = libcellml::Variable::create("g_Na");
        membrane->addVariable(g_Na);
        g_Na->setInterfaceType("public_and_private");
        g_Na->setUnits("mS_per_cm2");

        auto E_Na = libcellml::Variable::create("E_Na");
        membrane->addVariable(E_Na);
        E_Na->setInterfaceType("public_and_private");
        E_Na->setUnits("mV");
    }

    //  2.g Create concrete variables in the external parameters component, where their inital values will
    //      be set (eventually).  As you did in step 1.j you'll need to also add any units that these variables
    //      need into the controller model too.
    //      Create variables for h, m, E_Na, g_Na.
    //      Create the units for the g_Na term of milli Siemens per cm^2 and add them to the model.
    {
        auto h = libcellml::Variable::create("h");
        parameters->addVariable(h);
        h->setInterfaceType("public_and_private");
        h->setUnits("dimensionless");

        auto m = libcellml::Variable::create("m");
        parameters->addVariable(m);
        m->setInterfaceType("public_and_private");
        m->setUnits("dimensionless");

        auto E_Na = libcellml::Variable::create("E_Na");
        parameters->addVariable(E_Na);
        E_Na->setInterfaceType("public_and_private");
        E_Na->setUnits("mV");

        auto g_Na = libcellml::Variable::create("g_Na");
        parameters->addVariable(g_Na);
        g_Na->setInterfaceType("public_and_private");
        g_Na->setUnits("mS_per_cm2");

        // Add missing units to the controller model
        auto mS_per_cm2 = libcellml::Units::create("mS_per_cm2");
        mS_per_cm2->addUnit("siemens", "milli");
        mS_per_cm2->addUnit("metre", "centi", -2);
        controllerModel->addUnits(mS_per_cm2);
    }

    model->resolveImports("");
    assert(model->hasUnresolvedImports() == false);

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 3: Import the potassium channel component " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  3.a Repeat all the tasks in Step 2, this time for the potassium channel model you created in
    //      Tutorial 6.  If you did not complete Tutorial 6 you can use the tutorial6_PotassiumChannelModel.cellml
    //      from the resources folder, importing the component called "potassiumChannel"
    auto potassiumChannel = libcellml::Component::create("potassium_channel");
    membrane->addComponent(potassiumChannel);

    auto potassiumImporter = libcellml::ImportSource::create();
    potassiumImporter->setUrl("tutorial6_PotassiumChannelModel.cellml");

    potassiumChannel->setImportSource(potassiumImporter);
    potassiumChannel->setImportReference("potassiumChannel");

    model->resolveImports("");
    assert(model->hasUnresolvedImports() == false);

    // Local dummy variables:
    {
        auto V = libcellml::Variable::create("V");
        potassiumChannel->addVariable(V);
        auto t = libcellml::Variable::create("t");
        potassiumChannel->addVariable(t);
        auto i_K = libcellml::Variable::create("i_K");
        potassiumChannel->addVariable(i_K);

        auto n = libcellml::Variable::create("n");
        potassiumChannel->addVariable(n);
        auto g_K = libcellml::Variable::create("g_K");
        potassiumChannel->addVariable(g_K);
        auto E_K = libcellml::Variable::create("E_K");
        potassiumChannel->addVariable(E_K);
    }

    // Intermediate concrete variables in the membrane component:
    {
        auto n = libcellml::Variable::create("n");
        membrane->addVariable(n);
        auto g_K = libcellml::Variable::create("g_K");
        membrane->addVariable(g_K);
        auto E_K = libcellml::Variable::create("E_K");
        membrane->addVariable(E_K);

        n->setInterfaceType("public_and_private");
        g_K->setInterfaceType("public_and_private");
        E_K->setInterfaceType("public_and_private");
        n->setUnits("dimensionless");
        g_K->setUnits("milliS_per_cm2");
        E_K->setUnits("millivolt");
    }

    // Concrete variables in the external parameters component:
    {
        auto n = libcellml::Variable::create("n");
        parameters->addVariable(n);
        n->setInterfaceType("public_and_private");
        n->setUnits("dimensionless");

        auto g_K = libcellml::Variable::create("g_K");
        parameters->addVariable(g_K);
        g_K->setInterfaceType("public_and_private");
        g_K->setUnits("mS_per_cm2");

        auto E_K = libcellml::Variable::create("E_K");
        parameters->addVariable(E_K);
        E_K->setInterfaceType("public_and_private");
        E_K->setUnits("mV");
    }

    model->resolveImports("");
    assert(model->hasUnresolvedImports() == false);

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 4: Import the leakage component           " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  4.a Repeat all the tasks in Step 2, this time for the leakageCurrent component in
    //      the model supplied inside resources/tutorial8_LeakageModel.cellml.
    auto leakage = libcellml::Component::create("leakage");
    membrane->addComponent(leakage);

    auto leakageImporter = libcellml::ImportSource::create();
    leakageImporter->setUrl("tutorial8_LeakageCurrentModel.cellml");

    leakage->setImportSource(leakageImporter);
    leakage->setImportReference("leakageCurrent");

    // Dummy variables in the leakage component:
    {
        auto V = libcellml::Variable::create("V");
        leakage->addVariable(V);
        auto t = libcellml::Variable::create("t");
        leakage->addVariable(t);
        auto i_L = libcellml::Variable::create("i_L");
        leakage->addVariable(i_L);
        auto g_L = libcellml::Variable::create("g_L");
        leakage->addVariable(g_L);
        auto E_L = libcellml::Variable::create("E_L");
        leakage->addVariable(E_L);
    }

    // Concrete variables in the membrane component:
    {
        auto g_L = libcellml::Variable::create("g_L");
        g_L->setInterfaceType("public_and_private");
        g_L->setUnits("mS_per_cm2");
        membrane->addVariable(g_L);

        auto E_L = libcellml::Variable::create("E_L");
        E_L->setUnits("mV");
        E_L->setInterfaceType("public_and_private");
        membrane->addVariable(E_L);
    }

    // Concrete variables in the external parameters component:
    {
        auto g_L = libcellml::Variable::create("g_L");
        g_L->setInterfaceType("public_and_private");
        g_L->setUnits("mS_per_cm2");
        parameters->addVariable(g_L);

        auto E_L = libcellml::Variable::create("E_L");
        E_L->setUnits("mV");
        E_L->setInterfaceType("public_and_private");
        parameters->addVariable(E_L);
    }

    model->resolveImports("");
    assert(model->hasUnresolvedImports() == false);

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 5: Export the parameters                  " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  5.a Now that we've imported all the components and added the variables which
    //      need initial values into the parameters component, we can write the controller
    //      model (which contains that parameters component) to a separate file.
    //      This makes it easier to locate the parameters of interest and change them later.

    //      Set the initial conditions in the parameters component of:
    //      sodium channel
    //          E_Na(t=0) = 40
    //          g_Na(t=0) = 120
    //      h-gate
    //          h(t=0) = 0.6
    //      m-gate
    //          m(t=0) = 0.05
    //      potassium channel
    //          E_K(t=0) = -87
    //          g_K(t=0) = 36
    //      n-gate
    //          n(t=0) = 0.325
    //      leakage current
    //          g_L(t=0) = 0.3
    //          E_L(t=0) = -64.387
    //      membrane
    //          V(t=0) = -75
    //          Cm(t=0) = 1

    {
        // sodium channel
        parameters->variable("E_Na")->setInitialValue(40);
        parameters->variable("g_Na")->setInitialValue(120);
        // h-gate
        parameters->variable("h")->setInitialValue(0.6);
        // m-gate
        parameters->variable("m")->setInitialValue(0.05);

        // potassium channel
        parameters->variable("E_K")->setInitialValue(-87);
        parameters->variable("g_K")->setInitialValue(36);
        // n-gate
        parameters->variable("n")->setInitialValue(0.325);

        // leakage
        parameters->variable("g_L")->setInitialValue(0.3);
        parameters->variable("E_L")->setInitialValue(-64.387);

        // membrane
        parameters->variable("V")->setInitialValue(-75);
        parameters->variable("Cm")->setInitialValue(1);
    }

    //  5.b Validate the controller model and expect there to be no errors.  You may need to
    //      link the units if you find errors related to missing units.
    controllerModel->linkUnits();
    validator->validateModel(controllerModel);
    printErrorsToTerminal(validator);

    //  5.c Create a Printer instance, and serialise the controller model for output to a
    //      file.  Name your file appropriately - you will need to use its name to import
    //      it in step 6.
    auto printer = libcellml::Printer::create();
    std::ofstream outFile("tutorial8_HodgkinHuxley_controller.cellml");
    outFile << printer->printModel(controllerModel);
    outFile.close();

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 6: Import the parameters                  " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  6.a Repeat the tasks in Step 2.a-c, this time for the parameters component
    //      in the model which you wrote in step 5.c.
    //      You will need to use the same names for the file and the parameter
    //      component as you wrote earlier.
    parameters = libcellml::Component::create("parameters");
    model->addComponent(parameters);

    auto parametersImporter = libcellml::ImportSource::create();
    parametersImporter->setUrl("tutorial8_HodgkinHuxley_controller.cellml");

    parameters->setImportSource(parametersImporter);
    parameters->setImportReference("parameters");

    //  6.b Set up dummy variables for all of the variables in the parameters component
    //      so that they can be connected before flattening the model.
    //      Because these dummy variables will be overwritten, you do not need to specify
    //      the interface types, initial values, or units.
    {
        // Membrane component
        auto V = libcellml::Variable::create("V");
        parameters->addVariable(V);
        auto Cm = libcellml::Variable::create("Cm");
        parameters->addVariable(Cm);

        // Sodium channel
        auto h = libcellml::Variable::create("h");
        parameters->addVariable(h);
        auto m = libcellml::Variable::create("m");
        parameters->addVariable(m);
        auto E_Na = libcellml::Variable::create("E_Na");
        parameters->addVariable(E_Na);
        auto g_Na = libcellml::Variable::create("g_Na");
        parameters->addVariable(g_Na);

        // Potassium channel
        auto n = libcellml::Variable::create("n");
        parameters->addVariable(n);
        auto E_K = libcellml::Variable::create("E_K");
        parameters->addVariable(E_K);
        auto g_K = libcellml::Variable::create("g_K");
        parameters->addVariable(g_K);

        // Leakage
        auto E_L = libcellml::Variable::create("E_L");
        parameters->addVariable(E_L);
        auto g_L = libcellml::Variable::create("g_L");
        parameters->addVariable(g_L);
    }

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 7: Connect variables between components   " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  7.a Now that we've got all the imports done, we need to connect the imported
    //      components and their dummy variables together.  The variables to connect are:
    //          - voltage:  parameters -> membrane -> sodium channel, potassium channel, leakage
    //          - time: membrane -> sodium channel, potassium channel
    //          - current variables (i_Na, i_K, i_L): membrane -> channels
    //          - conductance variables (g_Na, g_K, g_L): membrane -> channels
    //          - potential variables (E_Na, E_K, E_L): membrane -> channels
    //          - gating variables (h, m, n): membrane -> channels
    //          - Cm: parameters -> membrane

    assert(libcellml::Variable::addEquivalence(parameters->variable("V"), membrane->variable("V")));
    assert(libcellml::Variable::addEquivalence(parameters->variable("Cm"), membrane->variable("Cm")));
    assert(libcellml::Variable::addEquivalence(parameters->variable("h"), membrane->variable("h")));
    assert(libcellml::Variable::addEquivalence(parameters->variable("m"), membrane->variable("m")));
    assert(libcellml::Variable::addEquivalence(parameters->variable("n"), membrane->variable("n")));
    assert(libcellml::Variable::addEquivalence(parameters->variable("g_K"), membrane->variable("g_K")));
    assert(libcellml::Variable::addEquivalence(parameters->variable("g_Na"), membrane->variable("g_Na")));
    assert(libcellml::Variable::addEquivalence(parameters->variable("g_L"), membrane->variable("g_L")));
    assert(libcellml::Variable::addEquivalence(parameters->variable("E_K"), membrane->variable("E_K")));
    assert(libcellml::Variable::addEquivalence(parameters->variable("E_Na"), membrane->variable("E_Na")));
    assert(libcellml::Variable::addEquivalence(parameters->variable("E_L"), membrane->variable("E_L")));

    assert(libcellml::Variable::addEquivalence(membrane->variable("V"), sodiumChannel->variable("V")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("t"), sodiumChannel->variable("t")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("i_Na"), sodiumChannel->variable("i_Na")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("E_Na"), sodiumChannel->variable("E_Na")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("g_Na"), sodiumChannel->variable("g_Na")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("h"), sodiumChannel->variable("h")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("m"), sodiumChannel->variable("m")));

    assert(libcellml::Variable::addEquivalence(membrane->variable("V"), potassiumChannel->variable("V")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("t"), potassiumChannel->variable("t")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("i_K"), potassiumChannel->variable("i_K")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("E_K"), potassiumChannel->variable("E_K")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("g_K"), potassiumChannel->variable("g_K")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("n"), potassiumChannel->variable("n")));

    assert(libcellml::Variable::addEquivalence(membrane->variable("V"), leakage->variable("V")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("i_L"), leakage->variable("i_L")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("g_L"), leakage->variable("g_L")));
    assert(libcellml::Variable::addEquivalence(membrane->variable("E_L"), leakage->variable("E_L")));

    //  7.b Serialise and write the model to a CellML file.  In the steps below the model will
    //      be flattened for code generation, but we need to keep an unflattened copy too.
    outFile.open("tutorial8_HodgkinHuxleyModel.cellml");
    outFile << printer->printModel(model);
    outFile.close();

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 8: Resolve imports and flatten the model  " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  8.a Resolve the model's imports to the folder where all of the files are located, and
    //      check that there are no unresolved imports outstanding.
    model->resolveImports("");
    assert(model->hasUnresolvedImports() == false);

    //  8.b Flatten the model, and print the flattened model structure to the terminal for checking.
    model->flatten();
    printEncapsulationStructureToTerminal(model);
    printModelToTerminal(model, false);

    //  8.c Validate the flattened model, expecting that there are no errors.
    validator->validateModel(model);
    printErrorsToTerminal(validator);
    assert(validator->errorCount() == 0);

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 9: Generate the model and output          " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  9.a Create a Generator instance and submit the model for processing.
    //      Expect that there are no errors logged in the generator afterwards.
    auto generator = libcellml::Generator::create();
    generator->processModel(model);
    printErrorsToTerminal(generator);

    assert(generator->errorCount() == 0);

    //  9.b Retrieve and write the interface code (*.h) and implementation code (*.c) to files.
    outFile.open("tutorial8_HodgkinHuxleyModel.h");
    outFile << generator->interfaceCode();
    outFile.close();

    outFile.open("tutorial8_HodgkinHuxleyModel.c");
    outFile << generator->implementationCode();
    outFile.close();

    //  9.c Change the generator profile to Python and reprocess the model.
    auto profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);
    generator->setProfile(profile);
    generator->processModel(model);

    //  9.d Retrieve and write the implementation code (*.py) to a file.
    outFile.open("tutorial8_HodgkinHuxleyModel.py");
    outFile << generator->implementationCode();
    outFile.close();

    std::cout << "The model has been output into tutorial8_HodgkinHuxleyModel.[c,h,py,cellml]"
              << std::endl;

    //  9.e Please seen the tutorial instructions for how to run this simulation using
    //      the simple solver provided.  Then go and have a cuppa, you're done!
}
