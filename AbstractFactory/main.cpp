#include <QCoreApplication>
#include "Unit.h"
#include <iostream>

std::string generateProgram(IFactory* factory ) {
    std::shared_ptr< ClassUnit > myClass = factory->getClass("MyClass");
    // C++  | static, const, virtual
    // Java | static, final, abstract, public, protected, private
    // C#   | const, virtual, static, public, private, protected private, protected, internal, protected internal
    myClass->add(
        factory->getMethod( "testFunc1", "void", MethodUnit::PUBLIC ),
        ClassUnit::PUBLIC
        );
    myClass->add(
        factory->getMethod( "testFunc2", "void", MethodUnit::STATIC | MethodUnit::PRIVATE),
        ClassUnit::PRIVATE
        );
    myClass->add(
        factory->getMethod( "testFunc3", "void", MethodUnit::CONST | MethodUnit::PROTECTED | MethodUnit::PRIVATE),
        ClassUnit::PROTECTED
        );
    std::shared_ptr< MethodUnit > method = factory->getMethod( "testFunc4", "void", MethodUnit::STATIC | MethodUnit::PROTECTED);
    method->add( factory->getPrintUnit( R"(Hello, world!\n)") );
    myClass->add( method, ClassUnit::PROTECTED );

    std::shared_ptr< MethodUnit > method2 = factory->getMethod( "TTTT1", "void", MethodUnit::PROTECTED);
    method2->add(factory->getMethod( "TTTT2", "void", MethodUnit::PROTECTED ));
    method2->add(factory->getMethod( "TTTT3", "void", MethodUnit::PROTECTED ));
    std::shared_ptr< MethodUnit > method3 = factory->getMethod( "TTTT4", "void", MethodUnit::PROTECTED);
    method3->add(factory->getMethod( "TTTT5", "void", MethodUnit::PROTECTED ));
    method2->add(method3);
    myClass->add( method2, ClassUnit::PROTECTED );


    return myClass->compile();
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CFactory Cplusplus;
    JavaFactory Java;
    SharpFactory Sharp;

    std::cout << generateProgram(&Cplusplus) << std::endl;
    std::cout << generateProgram(&Java) << std::endl;
    std::cout << generateProgram(&Sharp) << std::endl;

    return a.exec();
}
