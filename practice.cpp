// Student grade management system - practice file
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <map>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cmath>

// ============================================================
// SECTION 1: Enums and Constants
// ============================================================

enum class Grade {
    A = 90,
    B = 80,
    C = 70,
    D = 60,
    F = 0
};

enum class Department {
    ComputerScience,
    Mathematics,
    Physics,
    Chemistry
};

const int MAX_STUDENTS = 500;
const double PASSING_GRADE = 60.0;
const std::string GRADE_FILE = "grades.txt";

// ============================================================
// SECTION 2: Utility Functions
// ============================================================

std::string gradeToString(Grade g) {
    switch (g) {
        case Grade::A: return "A";
        case Grade::B: return "B";
        case Grade::C: return "C";
        case Grade::D: return "D";
        case Grade::F: return "F";
        default:       return "Unknown";
    }
}

std::string departmentToString(Department d) {
    switch (d) {
        case Department::ComputerScience: return "Computer Science";
        case Department::Mathematics:     return "Mathematics";
        case Department::Physics:         return "Physics";
        case Department::Chemistry:       return "Chemistry";
        default:                          return "Unknown";
    }
}

double clamp(double val, double minVal, double maxVal) {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

Grade scoreToGrade(double score) {
    if (score >= 90.0) return Grade::A;
    if (score >= 80.0) return Grade::B;
    if (score >= 70.0) return Grade::C;
    if (score >= 60.0) return Grade::D;
    return Grade::F;
}

bool isValidScore(double score) {
    return score >= 0.0 && score <= 100.0;
}

// ============================================================
// SECTION 3: Course class
// ============================================================

class Course {
public:
    Course(const std::string& code, const std::string& name, int credits)
        : m_code(code), m_name(name), m_credits(credits)
    {
        if (credits <= 0 || credits > 6)
            throw std::invalid_argument("Credits must be between 1 and 6");
        if (code.empty())
            throw std::invalid_argument("Course code cannot be empty");
    }

    const std::string& code()    const { return m_code; }
    const std::string& name()    const { return m_name; }
    int                credits() const { return m_credits; }

    bool operator==(const Course& other) const {
        return m_code == other.m_code;
    }

    bool operator<(const Course& other) const {
        return m_code < other.m_code;
    }

    std::string toString() const {
        return "[" + m_code + "] " + m_name + " (" + std::to_string(m_credits) + " credits)";
    }

private:
    std::string m_code;
    std::string m_name;
    int         m_credits;
};

// ============================================================
// SECTION 4: Enrollment record
// ============================================================

struct Enrollment {
    Course course;
    double midterm;
    double final_exam;
    double homework;

    Enrollment(const Course& c, double mid, double fin, double hw)
        : course(c), midterm(mid), final_exam(fin), homework(hw)
    {}

    double weighted_score() const {
        return midterm * 0.30 + final_exam * 0.50 + homework * 0.20;
    }

    Grade letter_grade() const {
        return scoreToGrade(weighted_score());
    }

    bool passed() const {
        return weighted_score() >= PASSING_GRADE;
    }
};

// ============================================================
// SECTION 5: Student class
// ============================================================

class Student {
public:
    Student(int id, const std::string& name, Department dept)
        : m_id(id), m_name(name), m_dept(dept)
    {
        if (id <= 0)
            throw std::invalid_argument("Student ID must be positive");
        if (name.empty())
            throw std::invalid_argument("Student name cannot be empty");
    }

    // Getters
    int                id()         const { return m_id; }
    const std::string& name()       const { return m_name; }
    Department         department() const { return m_dept; }

    // Enrollment
    void enroll(const Course& course, double midterm, double final_exam, double homework) {
        if (!isValidScore(midterm) || !isValidScore(final_exam) || !isValidScore(homework))
            throw std::out_of_range("Scores must be between 0 and 100");

        for (const auto& e : m_enrollments) {
            if (e.course == course)
                throw std::logic_error("Already enrolled in " + course.code());
        }

        m_enrollments.emplace_back(course, midterm, final_exam, homework);
    }

    // GPA calculation (4.0 scale)
    double gpa() const {
        if (m_enrollments.empty()) return 0.0;

        double total_points = 0.0;
        int    total_credits = 0;

        for (const auto& e : m_enrollments) {
            double score = e.weighted_score();
            double grade_points;

            if      (score >= 90) grade_points = 4.0;
            else if (score >= 80) grade_points = 3.0;
            else if (score >= 70) grade_points = 2.0;
            else if (score >= 60) grade_points = 1.0;
            else                  grade_points = 0.0;

            total_points  += grade_points * e.course.credits();
            total_credits += e.course.credits();
        }

        return total_credits > 0 ? total_points / total_credits : 0.0;
    }

    double average_score() const {
        if (m_enrollments.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& e : m_enrollments)
            sum += e.weighted_score();
        return sum / m_enrollments.size();
    }

    bool is_on_honor_roll() const { return gpa() >= 3.5; }
    bool is_failing()       const { return gpa() < 1.0; }

    int passed_courses()  const {
        int count = 0;
        for (const auto& e : m_enrollments)
            if (e.passed()) ++count;
        return count;
    }

    int failed_courses() const {
        return static_cast<int>(m_enrollments.size()) - passed_courses();
    }

    const std::vector<Enrollment>& enrollments() const { return m_enrollments; }

    void print_report() const {
        std::cout << "=== Student Report ===\n"
                  << "ID:         " << m_id   << "\n"
                  << "Name:       " << m_name << "\n"
                  << "Department: " << departmentToString(m_dept) << "\n"
                  << "GPA:        " << gpa()  << "\n"
                  << "Courses:\n";

        for (const auto& e : m_enrollments) {
            std::cout << "  " << e.course.toString()
                      << " -> " << gradeToString(e.letter_grade())
                      << " (" << e.weighted_score() << ")\n";
        }

        if (is_on_honor_roll()) std::cout << "** HONOR ROLL **\n";
        if (is_failing())       std::cout << "** ACADEMIC PROBATION **\n";
        std::cout << "=====================\n";
    }

private:
    int                      m_id;
    std::string              m_name;
    Department               m_dept;
    std::vector<Enrollment>  m_enrollments;
};

// ============================================================
// SECTION 6: Comparators for sorting
// ============================================================

struct CompareByGPA {
    bool operator()(const Student& a, const Student& b) const {
        return a.gpa() > b.gpa();   // descending
    }
};

struct CompareByName {
    bool operator()(const Student& a, const Student& b) const {
        return a.name() < b.name();
    }
};

struct CompareByID {
    bool operator()(const Student& a, const Student& b) const {
        return a.id() < b.id();
    }
};

// ============================================================
// SECTION 7: ClassRoom - container for students
// ============================================================

class ClassRoom {
public:
    explicit ClassRoom(const std::string& name) : m_name(name) {}

    void add_student(const Student& s) {
        if (m_students.size() >= MAX_STUDENTS)
            throw std::overflow_error("Classroom is full");
        for (const auto& existing : m_students) {
            if (existing.id() == s.id())
                throw std::invalid_argument("Student with this ID already exists");
        }
        m_students.push_back(s);
    }

    Student* find_student(int id) {
        for (auto& s : m_students)
            if (s.id() == id) return &s;
        return nullptr;
    }

    const Student* find_student(int id) const {
        for (const auto& s : m_students)
            if (s.id() == id) return &s;
        return nullptr;
    }

    void remove_student(int id) {
        auto it = std::remove_if(m_students.begin(), m_students.end(),
            [id](const Student& s){ return s.id() == id; });
        if (it == m_students.end())
            throw std::invalid_argument("Student not found");
        m_students.erase(it, m_students.end());
    }

    // Statistics
    double class_average() const {
        if (m_students.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& s : m_students)
            sum += s.average_score();
        return sum / m_students.size();
    }

    double class_gpa_average() const {
        if (m_students.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& s : m_students)
            sum += s.gpa();
        return sum / m_students.size();
    }

    int honor_roll_count() const {
        int count = 0;
        for (const auto& s : m_students)
            if (s.is_on_honor_roll()) ++count;
        return count;
    }

    int probation_count() const {
        int count = 0;
        for (const auto& s : m_students)
            if (s.is_failing()) ++count;
        return count;
    }

    // Sorted views
    std::vector<Student> sorted_by_gpa() const {
        std::vector<Student> copy = m_students;
        std::sort(copy.begin(), copy.end(), CompareByGPA{});
        return copy;
    }

    std::vector<Student> sorted_by_name() const {
        std::vector<Student> copy = m_students;
        std::sort(copy.begin(), copy.end(), CompareByName{});
        return copy;
    }

    // Filter
    std::vector<const Student*> honor_roll_students() const {
        std::vector<const Student*> result;
        for (const auto& s : m_students)
            if (s.is_on_honor_roll()) result.push_back(&s);
        return result;
    }

    std::vector<const Student*> students_in_department(Department d) const {
        std::vector<const Student*> result;
        for (const auto& s : m_students)
            if (s.department() == d) result.push_back(&s);
        return result;
    }

    // Department breakdown
    std::map<Department, int> department_counts() const {
        std::map<Department, int> counts;
        for (const auto& s : m_students)
            counts[s.department()]++;
        return counts;
    }

    // Top N students
    std::vector<Student> top_students(int n) const {
        std::vector<Student> sorted = sorted_by_gpa();
        if (n > static_cast<int>(sorted.size())) n = sorted.size();
        return std::vector<Student>(sorted.begin(), sorted.begin() + n);
    }

    void print_summary() const {
        std::cout << "=== Classroom: " << m_name << " ===\n"
                  << "Total students:  " << m_students.size()  << "\n"
                  << "Class average:   " << class_average()     << "\n"
                  << "Average GPA:     " << class_gpa_average() << "\n"
                  << "Honor roll:      " << honor_roll_count()  << "\n"
                  << "On probation:    " << probation_count()   << "\n";
    }

    const std::string&           name()     const { return m_name; }
    const std::vector<Student>&  students() const { return m_students; }
    int                          size()     const { return m_students.size(); }

    // Save to file
    void save_to_file(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open())
            throw std::runtime_error("Cannot open file: " + filename);

        file << m_name << "\n" << m_students.size() << "\n";
        for (const auto& s : m_students) {
            file << s.id() << ","
                 << s.name() << ","
                 << static_cast<int>(s.department()) << ","
                 << s.gpa() << "\n";
        }
    }

private:
    std::string          m_name;
    std::vector<Student> m_students;
};

// ============================================================
// SECTION 8: Statistics helper
// ============================================================

class Statistics {
public:
    static double mean(const std::vector<double>& data) {
        if (data.empty()) return 0.0;
        double sum = 0.0;
        for (double v : data) sum += v;
        return sum / data.size();
    }

    static double variance(const std::vector<double>& data) {
        if (data.size() < 2) return 0.0;
        double m = mean(data);
        double sum = 0.0;
        for (double v : data) sum += (v - m) * (v - m);
        return sum / (data.size() - 1);
    }

    static double std_dev(const std::vector<double>& data) {
        return std::sqrt(variance(data));
    }

    static double median(std::vector<double> data) {
        if (data.empty()) return 0.0;
        std::sort(data.begin(), data.end());
        int n = data.size();
        if (n % 2 == 0)
            return (data[n/2 - 1] + data[n/2]) / 2.0;
        return data[n/2];
    }

    static double percentile(std::vector<double> data, double p) {
        if (data.empty()) return 0.0;
        std::sort(data.begin(), data.end());
        double idx = p / 100.0 * (data.size() - 1);
        int lo = static_cast<int>(idx);
        int hi = lo + 1;
        if (hi >= static_cast<int>(data.size())) return data.back();
        double frac = idx - lo;
        return data[lo] * (1 - frac) + data[hi] * frac;
    }

    // Grade distribution
    static std::map<Grade, int> grade_distribution(const std::vector<double>& scores) {
        std::map<Grade, int> dist;
        for (double s : scores)
            dist[scoreToGrade(s)]++;
        return dist;
    }
};

// ============================================================
// SECTION 9: Report generator
// ============================================================

class ReportGenerator {
public:
    explicit ReportGenerator(const ClassRoom& classroom) : m_room(classroom) {}

    void generate_full_report() const {
        print_header();
        m_room.print_summary();
        print_separator();
        print_top_students(5);
        print_separator();
        print_grade_distribution();
        print_separator();
        print_department_breakdown();
        print_separator();
        print_honor_roll();
        print_footer();
    }

    void generate_student_report(int student_id) const {
        const Student* s = m_room.find_student(student_id);
        if (!s) {
            std::cout << "Student " << student_id << " not found.\n";
            return;
        }
        s->print_report();
    }

private:
    const ClassRoom& m_room;

    void print_header() const {
        std::cout << "\n";
        std::cout << "########################################\n";
        std::cout << "#      ACADEMIC PERFORMANCE REPORT     #\n";
        std::cout << "########################################\n\n";
    }

    void print_footer() const {
        std::cout << "\n########################################\n";
        std::cout << "#              END OF REPORT           #\n";
        std::cout << "########################################\n\n";
    }

    void print_separator() const {
        std::cout << "----------------------------------------\n";
    }

    void print_top_students(int n) const {
        std::cout << "TOP " << n << " STUDENTS BY GPA:\n";
        auto top = m_room.top_students(n);
        int rank = 1;
        for (const auto& s : top) {
            std::cout << "  " << rank++ << ". "
                      << s.name() << " (ID: " << s.id() << ")"
                      << " - GPA: " << s.gpa() << "\n";
        }
    }

    void print_grade_distribution() const {
        std::cout << "GRADE DISTRIBUTION:\n";
        std::vector<double> scores;
        for (const auto& s : m_room.students())
            scores.push_back(s.average_score());

        auto dist = Statistics::grade_distribution(scores);
        for (const auto& [grade, count] : dist) {
            std::cout << "  " << gradeToString(grade) << ": " << count << " students\n";
        }

        if (!scores.empty()) {
            std::cout << "  Mean:   " << Statistics::mean(scores)   << "\n"
                      << "  Median: " << Statistics::median(scores) << "\n"
                      << "  StdDev: " << Statistics::std_dev(scores)<< "\n";
        }
    }

    void print_department_breakdown() const {
        std::cout << "DEPARTMENT BREAKDOWN:\n";
        auto counts = m_room.department_counts();
        for (const auto& [dept, count] : counts) {
            std::cout << "  " << departmentToString(dept)
                      << ": " << count << " students\n";
        }
    }

    void print_honor_roll() const {
        std::cout << "HONOR ROLL (GPA >= 3.5):\n";
        auto hr = m_room.honor_roll_students();
        if (hr.empty()) {
            std::cout << "  (none)\n";
            return;
        }
        for (const auto* s : hr)
            std::cout << "  * " << s->name() << " - GPA: " << s->gpa() << "\n";
    }
};

// ============================================================
// SECTION 10: Simple template - generic stack
// ============================================================

template <typename T>
class Stack {
public:
    void push(const T& val) {
        m_data.push_back(val);
    }

    void pop() {
        if (empty())
            throw std::underflow_error("Stack is empty");
        m_data.pop_back();
    }

    T& top() {
        if (empty())
            throw std::underflow_error("Stack is empty");
        return m_data.back();
    }

    const T& top() const {
        if (empty())
            throw std::underflow_error("Stack is empty");
        return m_data.back();
    }

    bool empty() const { return m_data.empty(); }
    int  size()  const { return m_data.size(); }

    void clear() { m_data.clear(); }

private:
    std::vector<T> m_data;
};

// ============================================================
// SECTION 11: Unit tests (simple assertions)
// ============================================================

void test_course_creation() {
    Course c("CS101", "Intro to CS", 3);
    assert(c.code()    == "CS101");
    assert(c.name()    == "Intro to CS");
    assert(c.credits() == 3);

    bool threw = false;
    try { Course bad("", "name", 3); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw && "Empty code should throw");

    threw = false;
    try { Course bad("X", "name", 0); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw && "Zero credits should throw");

    std::cout << "[PASS] test_course_creation\n";
}

void test_enrollment_scoring() {
    Course c("MATH201", "Calculus", 4);
    Enrollment e(c, 80.0, 70.0, 90.0);

    // weighted = 80*0.3 + 70*0.5 + 90*0.2 = 24 + 35 + 18 = 77
    assert(std::abs(e.weighted_score() - 77.0) < 0.001);
    assert(e.letter_grade() == Grade::C);
    assert(e.passed());

    std::cout << "[PASS] test_enrollment_scoring\n";
}

void test_student_gpa() {
    Student s(1001, "Alice", Department::ComputerScience);
    Course c1("CS101", "Intro CS",    3);
    Course c2("MATH101", "Algebra",   3);

    // cs: mid=90, fin=90, hw=90 -> score=90 -> 4.0 * 3 = 12
    // math: mid=70, fin=60, hw=80 -> score=67 -> 1.0 * 3 = 3
    // gpa = (12 + 3) / 6 = 2.5
    s.enroll(c1, 90, 90, 90);
    s.enroll(c2, 70, 60, 80);

    assert(std::abs(s.gpa() - 2.5) < 0.001);
    assert(s.passed_courses() == 2);
    assert(!s.is_on_honor_roll());
    assert(!s.is_failing());

    std::cout << "[PASS] test_student_gpa\n";
}

void test_duplicate_enrollment() {
    Student s(1002, "Bob", Department::Mathematics);
    Course c("CS101", "Intro CS", 3);
    s.enroll(c, 75, 80, 70);

    bool threw = false;
    try { s.enroll(c, 80, 85, 90); }
    catch (const std::logic_error&) { threw = true; }
    assert(threw && "Duplicate enrollment should throw");

    std::cout << "[PASS] test_duplicate_enrollment\n";
}

void test_statistics() {
    std::vector<double> data = {60, 70, 80, 90, 100};
    assert(Statistics::mean(data)   == 80.0);
    assert(Statistics::median(data) == 80.0);

    std::vector<double> data2 = {2, 4, 4, 4, 5, 5, 7, 9};
    // sample std dev with n-1: sqrt(32/7) ≈ 2.1381
    assert(std::abs(Statistics::std_dev(data2) - 2.1381) < 0.001);

    std::cout << "[PASS] test_statistics\n";
}

void test_stack() {
    Stack<int> st;
    assert(st.empty());

    st.push(1);
    st.push(2);
    st.push(3);

    assert(st.size() == 3);
    assert(st.top()  == 3);

    st.pop();
    assert(st.top() == 2);

    st.clear();
    assert(st.empty());

    bool threw = false;
    try { st.pop(); }
    catch (const std::underflow_error&) { threw = true; }
    assert(threw && "Pop on empty stack should throw");

    std::cout << "[PASS] test_stack\n";
}

void test_classroom_operations() {
    ClassRoom room("CS Department");
    Student s1(1, "Alice",   Department::ComputerScience);
    Student s2(2, "Bob",     Department::Mathematics);
    Student s3(3, "Charlie", Department::ComputerScience);

    Course cs101("CS101", "Intro CS",  3);
    Course math101("MATH101", "Math",  3);

    // Enroll students
    s1.enroll(cs101,   95, 92, 98);  // high scores -> honor roll
    s2.enroll(math101, 55, 50, 60);  // low scores  -> failing
    s3.enroll(cs101,   75, 70, 80);  // mid scores

    room.add_student(s1);
    room.add_student(s2);
    room.add_student(s3);

    assert(room.size()           == 3);
    assert(room.honor_roll_count() == 1);

    auto top = room.top_students(2);
    assert(top[0].id() == 1);  // Alice should be first

    room.remove_student(2);
    assert(room.size() == 2);
    assert(room.find_student(2) == nullptr);

    std::cout << "[PASS] test_classroom_operations\n";
}

void run_all_tests() {
    std::cout << "\n=== Running Tests ===\n";
    test_course_creation();
    test_enrollment_scoring();
    test_student_gpa();
    test_duplicate_enrollment();
    test_statistics();
    test_stack();
    test_classroom_operations();
    std::cout << "=== All tests passed ===\n\n";
}

// ============================================================
// SECTION 12: Main
// ============================================================

int main() {
    run_all_tests();

    // --- Build sample data ---
    ClassRoom room("Spring 2026");

    Course cs101  ("CS101",   "Intro to Programming", 3);
    Course cs201  ("CS201",   "Data Structures",      3);
    Course math101("MATH101", "Calculus I",            4);
    Course phys101("PHYS101", "Physics I",             4);
    Course chem101("CHEM101", "Chemistry I",           3);

    // Students
    std::vector<Student> students = {
        Student(1001, "Alice Johnson",   Department::ComputerScience),
        Student(1002, "Bob Smith",       Department::Mathematics),
        Student(1003, "Carol White",     Department::Physics),
        Student(1004, "David Brown",     Department::ComputerScience),
        Student(1005, "Eve Davis",       Department::Chemistry),
        Student(1006, "Frank Wilson",    Department::Mathematics),
        Student(1007, "Grace Lee",       Department::ComputerScience),
        Student(1008, "Henry Martinez",  Department::Physics),
    };

    // Enroll students with varied scores
    students[0].enroll(cs101,   95, 92, 98);
    students[0].enroll(cs201,   88, 90, 95);
    students[0].enroll(math101, 82, 85, 80);

    students[1].enroll(math101, 78, 82, 75);
    students[1].enroll(phys101, 65, 70, 68);

    students[2].enroll(phys101, 91, 88, 93);
    students[2].enroll(math101, 85, 87, 90);

    students[3].enroll(cs101,   55, 50, 60);
    students[3].enroll(cs201,   48, 52, 55);

    students[4].enroll(chem101, 72, 68, 75);
    students[4].enroll(math101, 60, 58, 65);

    students[5].enroll(math101, 88, 90, 85);
    students[5].enroll(phys101, 80, 78, 82);

    students[6].enroll(cs101,   97, 95, 99);
    students[6].enroll(cs201,   94, 96, 92);

    students[7].enroll(phys101, 40, 35, 45);
    students[7].enroll(chem101, 38, 42, 40);

    for (auto& s : students)
        room.add_student(s);

    // Generate report
    ReportGenerator reporter(room);
    reporter.generate_full_report();

    // Individual report
    reporter.generate_student_report(1001);
    reporter.generate_student_report(9999);  // not found

    // Stack demo
    std::cout << "=== Stack Demo ===\n";
    Stack<std::string> name_stack;
    name_stack.push("Alice");
    name_stack.push("Bob");
    name_stack.push("Carol");

    while (!name_stack.empty()) {
        std::cout << "Popped: " << name_stack.top() << "\n";
        name_stack.pop();
    }

    return 0;
}
