USE odb_lab17;

SET NAMES utf8mb4;

-- 清理之前插入的空姓名记录
DELETE FROM tbl_student WHERE name = '';

-- 插入班级
INSERT INTO tbl_classes(id, name)
VALUES
(1, '一年级一班'),
(2, '一年级二班');

-- 插入学生
INSERT INTO tbl_student(classes_id, name)
VALUES
(1, '张三'),
(1, '李四'),
(1, '王五'),
(2, '赵六');

-- 验证联表查询
SELECT
    c.name AS class_name,
    s.name AS student_name
FROM tbl_classes AS c
JOIN tbl_student AS s
    ON c.id = s.classes_id
WHERE c.id = 1
ORDER BY s.sn;
