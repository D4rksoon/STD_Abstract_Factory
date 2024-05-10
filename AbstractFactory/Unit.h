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
    virtual void add( const std::shared_ptr< Unit >& , Flags ) {
        throw std::runtime_error( "Not supported" );
    }
    virtual std::string compile( unsigned int level = 0 ) const =
        0;
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

// класс нашего класса
class ClassUnit : public Unit
{
public:
    enum AccessModifier { // Перечисление (определение типов)
        PUBLIC, // эквивалент 0
        PROTECTED, // эквивалент 1
        PRIVATE
    };
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
        result += generateShift( level ) + "};\n"; // в самом конце класса отступ
        return result;
    }
private:
    std::string m_name;
    using Fields = std::vector< std::shared_ptr< Unit > >;
    std::vector< Fields > m_fields; // вектор из 3 векторов
}; // Выводится в консоль начиная с public функций, заканчивая private (можно поменять)
const std::vector< std::string > ClassUnit::ACCESS_MODIFIERS = { "public", "protected", "private" };

// Класс функций (методов)
class MethodUnit : public Unit {
public:
    enum Modifier { // Перечисление (битовые флаги)
        STATIC = 1,
        CONST = 1 << 1,
        VIRTUAL = 1 << 2
    };
public:
    MethodUnit( const std::string& name, const std::string& returnType, Flags flags ) :
        m_name( name ), m_returnType( returnType ), m_flags( flags ) { }
    // добваить в метод другие методы
    // Flags - не знаю зачем нужен. Ни на что не влияет (можно удалить)
    void add( const std::shared_ptr< Unit >& unit, Flags /* flags */ = 0 ) {
        m_body.push_back( unit );
    }
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
private:
    std::string m_name;
    std::string m_returnType;
    Flags m_flags;
    std::vector< std::shared_ptr< Unit > > m_body;
};

// Добавляет в метод описание printf("text")
class PrintOperatorUnit : public Unit {
public:
    explicit PrintOperatorUnit( const std::string& text ) : m_text( text ) { }
    std::string compile( unsigned int level = 0 ) const {
        return generateShift( level ) + "printf( \"" + m_text + "\" );\n";
    }
private:
    std::string m_text;
};


#endif // UNIT_H
