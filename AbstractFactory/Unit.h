#ifndef UNIT_H
#define UNIT_H
#include <string>
#include <vector>
#include <memory>

class Unit {
public:
    using Flags = unsigned int; // переобозначение
public:
    virtual ~Unit() = default;
    // Умные указатели обеспечивают безопасность за счёт автоматического управления памятью
    virtual void add( const std::shared_ptr< Unit >& , Flags ) {
        throw std::runtime_error( "Not supported" );
    }
    virtual std::string compile( unsigned int level = 0 ) const = 0;
protected:
    virtual std::string generateShift( unsigned int level ) const
    {
        static const auto DEFAULT_SHIFT = "    ";
        std::string result;
        for( unsigned int i = 0; i < level; ++i ) {
            result += DEFAULT_SHIFT;
        }
        return result;
    }
};


class ClassUnit : public Unit
{
public:
    enum AccessModifier { // Перечисление (определение типов)
        PUBLIC, // эквивалент 0
        PROTECTED, // эквивалент 1
        PRIVATE
    };
    // статический вектор (доступен во всей программе)
    static const std::vector< std::string > ACCESS_MODIFIERS;
public:
    // explicit - запрещает неявные преобразования
    explicit ClassUnit( const std::string& name ) : m_name( name ) {
        m_fields.resize( ACCESS_MODIFIERS.size() );
    }
    // Flags - переобозначили (unsigned int)
    void add( const std::shared_ptr< Unit >& unit, Flags flags ) {
        // По умолчанию accessModifier стоит PUBLIC
        int accessModifier = PRIVATE; // По умолчанию ставим PRIVATE
        if( flags < ACCESS_MODIFIERS.size() ) { // Если flags меньше вектора известных нам
            accessModifier = flags;             // модификаторов ставим модификатор
        }
        m_fields[ accessModifier ].push_back( unit );
    }

protected:
    std::string m_name;
    using Fields = std::vector< std::shared_ptr< Unit > >;
    std::vector< Fields > m_fields; // вектор из 3 векторов
}; // Выводится в консоль начиная с public функций, заканчивая private (можно поменять)
const std::vector< std::string > ClassUnit::ACCESS_MODIFIERS = { "public", "protected", "private" };

class MethodUnit : public Unit
{
public:
    enum Modifier { // Перечисление (битовые флаги)
        STATIC = 1,
        CONST = 1 << 1,
        VIRTUAL = 1 << 2,
        FINAL = 1 << 3,
        ABSTRACT = 1 << 4,
        PUBLIC = 1 << 5,
        PROTECTED = 1 << 6,
        PRIVATE = 1 << 7,
        INTERNAL = 1 << 8
    };
public:
    MethodUnit( const std::string& name, const std::string& returnType, Flags flags ) :
        m_name( name ), m_returnType( returnType ), m_flags( flags ) { }
    // добваить в метод другие методы
    // Flags - не знаю зачем нужен. Ни на что не влияет (можно удалить)
    void add( const std::shared_ptr< Unit >& unit, Flags /* flags */ = 0 ) {
        m_body.push_back( unit );
    }

protected:
    std::string m_name;
    std::string m_returnType;
    Flags m_flags;
    std::vector< std::shared_ptr< Unit > > m_body;
};

class PrintUnit : public Unit
{
public:
    explicit PrintUnit( const std::string& text ) : m_text( text ) { }
protected:
    std::string m_text;
};



class CClassUnit : public ClassUnit
{
public:
    CClassUnit(const std::string& name)
        : ClassUnit(name){}
    // level - количестов отступов level * shift
    std::string compile( unsigned int level = 0 ) const
    {
        std::string result = generateShift( level ) + "class " + m_name + " {\n";
        // Выводится в консоль начиная с public функций, заканчивая private
        for( size_t i = 0; i < ACCESS_MODIFIERS.size(); ++i ) {
            if( m_fields[ i ].empty() ) {
                continue; // если вектор пуст пропускаем, идем на следующий
            }
            result += ACCESS_MODIFIERS[ i ] + ":\n";
            for( const auto& f : m_fields[ i ] ) { // идем по элементам вектора
                // f это MethodUnit использует compile из класса MethodUnit
                result += f->compile( level + 1 );
            }
            result += "\n"; // после обхода вектора в конце отступ
        }
        result += generateShift( level ) + "};\n\n"; // в самом конце класса отступ
        return result;
    }
};

class CMethodUnit : public MethodUnit
{
public:
    enum Modifier { // Перечисление (битовые флаги)
        STATIC = 1,
        CONST = 1 << 1,
        VIRTUAL = 1 << 2
    };
public:
    CMethodUnit(const std::string& name, const std::string& returnType, Flags flags)
        : MethodUnit(name, returnType, flags){}
    // вывести метод
    std::string compile( unsigned int level = 0 ) const {
        std::string result = generateShift( level );
        // Если флаг совпадает, добавляем
        if( m_flags & STATIC ) {
            result += "static ";
        } else if( m_flags & VIRTUAL ) {
            result += "virtual ";
        }

        result += m_returnType + " "; // Тип метода
        result += m_name + "()"; // Имя метода

        // Модификаторы, которые идут после объявления
        if( m_flags & CONST ) {
            result += " const";
        }
        result += " {\n";
        // Если в метод добавили ещё метод (проходим по телу метода)
        for( const auto& b : m_body ) {
            // b - MethodUnit | PrintOperatorUnit
            // Отступ увеличили на один shift
            result += b->compile( level + 1 );
        }
        result += generateShift( level ) + "}\n";
        return result;
    }
};

class CPrintUnit : public PrintUnit
{
public:
    CPrintUnit( const std::string& text )
        : PrintUnit(text){}
    std::string compile(unsigned int level = 0) const {
        return generateShift( level ) + "printf( \"" + m_text + "\" );\n";
    }
};



class JavaClassUnit : public ClassUnit
{
public:
    JavaClassUnit(const std::string& name)
        : ClassUnit(name){}
    // level - количестов отступов level * shift
    std::string compile( unsigned int level = 0 ) const
    {
        std::string result = generateShift( level ) + "class " + m_name + " {\n";
        for( size_t i = 0; i < ACCESS_MODIFIERS.size(); ++i ) {
            if( m_fields[ i ].empty() ) {
                continue; // если вектор пуст пропускаем, идем на следующий
            }
            for( const auto& f : m_fields[ i ] ) { // идем по элементам вектора
                // f это MethodUnit использует compile из класса MethodUnit
                result += f->compile( level + 1 );
            }
            //result += "\n"; // после обхода вектора в конце отступ
        }
        result += generateShift( level ) + "};\n\n"; // в самом конце класса отступ
        return result;
    }
};

class JavaMethodUnit : public MethodUnit
{
public:
    enum Modifier { // Перечисление (битовые флаги)
        STATIC = 1,
        FINAL = 1 << 3,
        ABSTRACT = 1 << 4,
        PUBLIC = 1 << 5,
        PROTECTED = 1 << 6,
        PRIVATE = 1 << 7
    };
public:
    JavaMethodUnit(const std::string& name, const std::string& returnType, Unit::Flags flags)
        : MethodUnit(name, returnType, flags){}
    // вывести метод
    std::string compile( unsigned int level = 0 ) const {
        std::string result = generateShift( level );
        // Если флаг совпадает, добавляем
        if (m_flags & PUBLIC) {
            result += "public ";
        } else if (m_flags & PROTECTED) {
            result += "protected ";
        } else if (m_flags & PRIVATE) {
            result += "private ";
        }

        if( m_flags & STATIC ) {
            result += "static ";
        } else if( m_flags & FINAL ) {
            result += "final ";
        } else if (m_flags & ABSTRACT) {
            result += "abstract ";
        }

        result += m_returnType + " "; // Тип метода
        result += m_name + "()"; // Имя метода

        result += " {\n";
        // Если в метод добавили ещё метод (проходим по телу метода)
        for( const auto& b : m_body ) {
            // b - MethodUnit | PrintOperatorUnit
            // Отступ увеличили на один shift
            result += b->compile( level + 1 );
        }
        result += generateShift( level ) + "}\n";
        return result;
    }
};

class JavaPrintUnit : public PrintUnit
{
public:
    JavaPrintUnit(const std::string& text)
        : PrintUnit(text){}
    std::string compile( unsigned int level = 0 ) const
    {
        return generateShift(level) + "System.out.println( \"" + m_text + "\" );\n";
    }
};



class SharpClassUnit : public ClassUnit
{
public:
    SharpClassUnit(const std::string& name)
        : ClassUnit(name){}
    // level - количестов отступов level * shift
    std::string compile( unsigned int level = 0 ) const
    {
        std::string result = generateShift( level ) + "class " + m_name + " {\n";
        for( size_t i = 0; i < ACCESS_MODIFIERS.size(); ++i ) {
            if( m_fields[ i ].empty() ) {
                continue; // если вектор пуст пропускаем, идем на следующий
            }
            for( const auto& f : m_fields[ i ] ) { // идем по элементам вектора
                // f это MethodUnit использует compile из класса MethodUnit
                result += f->compile( level + 1 );
            }
        }
        result += generateShift( level ) + "};\n\n"; // в самом конце класса отступ
        return result;
    }
};

class SharpMethodUnit : public MethodUnit
{
public:
    enum Modifier { // Перечисление (битовые флаги)
        STATIC = 1,
        CONST = 1 << 1,
        VIRTUAL = 1 << 2,
        PUBLIC = 1 << 5,
        PROTECTED = 1 << 6,
        PRIVATE = 1 << 7,
        INTERNAL = 1 << 8
    };
public:
    SharpMethodUnit(const std::string& name, const std::string& returnType, Unit::Flags flags)
        : MethodUnit(name, returnType, flags){}
    // вывести метод
    std::string compile( unsigned int level = 0 ) const {
        std::string result = generateShift( level );
        // Если флаг совпадает, добавляем
        if (m_flags & PUBLIC) {
            result += "public ";
        } else {
            if (m_flags & PROTECTED) {
                result += "protected ";
            }

            if (m_flags & PRIVATE) {
                result += "private ";
            } else if (m_flags & INTERNAL) {
                result += "internal ";
            }
        }

        if( m_flags & STATIC ) {
            result += "static ";
        } else if( m_flags & CONST ) {
            result += "const ";
        } else if (m_flags & VIRTUAL) {
            result += "virtual ";
        }
        result += m_returnType + " "; // Тип метода
        result += m_name + "()"; // Имя метода

        result += " {\n";
        // Если в метод добавили ещё метод (проходим по телу метода)
        for( const auto& b : m_body ) {
            // b - MethodUnit | PrintOperatorUnit
            // Отступ увеличили на один shift
            result += b->compile( level + 1 );
        }
        result += generateShift( level ) + "}\n";
        return result;
    }
};

class SharpPrintUnit : public PrintUnit
{
public:
    SharpPrintUnit(const std::string& text)
        : PrintUnit(text){}
    std::string compile( unsigned int level = 0 ) const
    {
        return generateShift(level) + "Console.WriteLine( $\"" + m_text + "\" );\n";
    }
};



class IFactory
{
public:
    virtual std::shared_ptr<ClassUnit> getClass( const std::string& name ) = 0;
    virtual std::shared_ptr<MethodUnit> getMethod( const std::string& name, const std::string& returnType, Unit::Flags flags ) = 0;
    virtual std::shared_ptr<PrintUnit> getPrintUnit( const std::string& text ) = 0;
};

class CFactory : public IFactory
{
public:
    std::shared_ptr<ClassUnit> getClass( const std::string& name )
    {
        return std::shared_ptr<ClassUnit>(new CClassUnit(name));
    }
    std::shared_ptr<MethodUnit> getMethod( const std::string &name, const std::string &returnType, Unit::Flags flags )
    {
        return std::shared_ptr<MethodUnit>(new CMethodUnit(name, returnType, flags));
    }
    std::shared_ptr<PrintUnit> getPrintUnit( const std::string &text )
    {
        return std::shared_ptr<PrintUnit>(new CPrintUnit(text));
    }
};

class JavaFactory : public IFactory
{
public:
    std::shared_ptr<ClassUnit> getClass( const std::string& name )
    {
        return std::shared_ptr<ClassUnit>(new JavaClassUnit(name));
    }
    std::shared_ptr<MethodUnit> getMethod( const std::string &name, const std::string &returnType, Unit::Flags flags )
    {
        return std::shared_ptr<MethodUnit>(new JavaMethodUnit(name, returnType, flags));
    }
    std::shared_ptr<PrintUnit> getPrintUnit( const std::string &text )
    {
        return std::shared_ptr<PrintUnit>(new JavaPrintUnit(text));
    }
};

class SharpFactory : public IFactory
{
public:
    std::shared_ptr<ClassUnit> getClass( const std::string& name )
    {
        return std::shared_ptr<ClassUnit>(new SharpClassUnit(name));
    }
    std::shared_ptr<MethodUnit> getMethod( const std::string &name, const std::string &returnType, Unit::Flags flags )
    {
        return std::shared_ptr<MethodUnit>(new SharpMethodUnit(name, returnType, flags));
    }
    std::shared_ptr<PrintUnit> getPrintUnit( const std::string &text )
    {
        return std::shared_ptr<PrintUnit>(new SharpPrintUnit(text));
    }
};

#endif // UNIT_H
