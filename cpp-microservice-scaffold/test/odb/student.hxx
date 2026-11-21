
#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include <odb/core.hxx>

// 学生表
#pragma db object table("tbl_student")
class Student {
public:
    Student() = default;

    std::size_t sn() const { return _sn; }
    const std::string& name() const { return _name; }

private:
    friend class odb::access;

    #pragma db id auto
    std::size_t _sn = 0;

    #pragma db index
    std::size_t _classes_id = 0;

    std::string _name;
};

// 班级表
#pragma db object table("tbl_classes")
class Classes {
public:
    Classes() = default;

    std::size_t id() const { return _id; }
    const std::string& name() const { return _name; }

private:
    friend class odb::access;

    #pragma db id auto
    std::size_t _id = 0;

    #pragma db unique type("VARCHAR(32)")
    std::string _name;
};

// 班级与学生的关联视图
#pragma db view object(Classes) \
    object(Student : Student::_classes_id == Classes::_id) \
    query((?))
struct ClassesStudent {
    #pragma db column(Classes::_name)
    std::string classes_name;

    #pragma db column(Student::_name)
    std::string student_name;
};
