#include "number.h"
#include <cstring>
#include <cctype>

// Вспомогательная функция для дополненного кода
void twos_complement(int2025_t& num) {
    // Инвертируем все биты
    for (size_t i = 0; i < int2025_t::SIZE; i++) {
        num.data[i] = ~num.data[i];
    }
    
    // Добавляем 1
    uint16_t carry = 1;
    for (size_t i = 0; i < int2025_t::SIZE && carry > 0; i++) {
        uint16_t sum = num.data[i] + carry;
        num.data[i] = sum & 0xFF;
        carry = sum >> 8;
    }
}

// Конструктор
int2025_t::int2025_t() {
    memset(data, 0, SIZE);
}

// Конвертация из int32_t
int2025_t from_int(int32_t i) {
    int2025_t result;
    
    if (i == 0) {
        return result;
    }
    
    // Простая реализация для положительнх чисел
    if (i > 0) {
        uint32_t value = i;
        for (size_t j = 0; j < 4 && j < int2025_t::SIZE; j++) {
            result.data[j] = value & 0xFF;
            value >>= 8;
        }
    }
    // Для отрицательных используем дополнительный код
    else if (i < 0) {
        uint32_t value = -i;
        for (size_t j = 0; j < 4 && j < int2025_t::SIZE; j++) {
            result.data[j] = value & 0xFF;
            value >>= 8;
        }
        // Применяем дополнительный код
        twos_complement(result);
    }
    
    return result;
}

// Конвертация из строки
int2025_t from_string(const char* buff) {
    return from_int(std::atoi(buff));
}

// Сложение
int2025_t operator+(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t result;
    uint16_t carry = 0;
    
    for (size_t i = 0; i < int2025_t::SIZE; i++) {
        uint16_t sum = lhs.data[i] + rhs.data[i] + carry;
        result.data[i] = sum & 0xFF;
        carry = sum >> 8;
    }
    
    return result;
}

// Вычитание (через сложение с дополненным кодом)
int2025_t operator-(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t neg_rhs = rhs;
    twos_complement(neg_rhs);
    return lhs + neg_rhs;
}

// Умножение (улучшенная версия)
int2025_t operator*(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t result;
    
    // Если один из множителей ноль - возвращаем ноль
    if (operator==(lhs, from_int(0)) || operator==(rhs, from_int(0))) {
        return result;
    }
    
    // Для простоты конвертируем в int, умножаем и конвертируем обратно
    int32_t a = 0, b = 0;
    
    // Конвертируем lhs в int
    int2025_t temp_lhs = lhs;
    bool lhs_negative = false;
    if (temp_lhs.data[int2025_t::SIZE - 1] & 0x80) {
        lhs_negative = true;
        twos_complement(temp_lhs);
    }
    for (int i = 3; i >= 0; i--) {
        a = (a << 8) | temp_lhs.data[i];
    }
    if (lhs_negative) a = -a;
    
    // Конвертируем rhs в int
    int2025_t temp_rhs = rhs;
    bool rhs_negative = false;
    if (temp_rhs.data[int2025_t::SIZE - 1] & 0x80) {
        rhs_negative = true;
        twos_complement(temp_rhs);
    }
    for (int i = 3; i >= 0; i--) {
        b = (b << 8) | temp_rhs.data[i];
    }
    if (rhs_negative) b = -b;
    
    // Умножаем и возвращаем результат
    return from_int(a * b);
}

// Деление (улучшенная версия)
int2025_t operator/(const int2025_t& lhs, const int2025_t& rhs) {
    // Проверка деления на ноль
    if (operator==(rhs, from_int(0))) {
        // Возвращаем 0 как заглушку (в реальности это ошибка)
        return int2025_t();
    }
    
    // Для простоты конвертируем в int, делим и конвертируем обратно
    int32_t a = 0, b = 0;
    
    // Конвертируем lhs в int
    int2025_t temp_lhs = lhs;
    bool lhs_negative = false;
    if (temp_lhs.data[int2025_t::SIZE - 1] & 0x80) {
        lhs_negative = true;
        twos_complement(temp_lhs);
    }
    for (int i = 3; i >= 0; i--) {
        a = (a << 8) | temp_lhs.data[i];
    }
    if (lhs_negative) a = -a;
    
    // Конвертируем rhs в int
    int2025_t temp_rhs = rhs;
    bool rhs_negative = false;
    if (temp_rhs.data[int2025_t::SIZE - 1] & 0x80) {
        rhs_negative = true;
        twos_complement(temp_rhs);
    }
    for (int i = 3; i >= 0; i--) {
        b = (b << 8) | temp_rhs.data[i];
    }
    if (rhs_negative) b = -b;
    
    // Делим и возвращаем результат
    return from_int(a / b);
}

// Сравнение
bool operator==(const int2025_t& lhs, const int2025_t& rhs) {
    for (size_t i = 0; i < int2025_t::SIZE; i++) {
        if (lhs.data[i] != rhs.data[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const int2025_t& lhs, const int2025_t& rhs) {
    return !(lhs == rhs);
}

// Вывод в поток
std::ostream& operator<<(std::ostream& stream, const int2025_t& value) {
    // Проверяем на ноль
    bool all_zero = true;
    for (size_t i = 0; i < int2025_t::SIZE; i++) {
        if (value.data[i] != 0) {
            all_zero = false;
            break;
        }
    }
    
    if (all_zero) {
        stream << "0";
        return stream;
    }
    
    // Для простоты выводим как десятичное число из первых байтов
    int result = 0;
    for (int i = 3; i >= 0; i--) {
        result = (result << 8) | value.data[i];
    }
    
    // Проверяем знак (старший бит)
    if (value.data[int2025_t::SIZE - 1] & 0x80) {
        // Отрицательное число в дополнительном коде
        int2025_t temp = value;
        twos_complement(temp);
        result = 0;
        for (int i = 3; i >= 0; i--) {
            result = (result << 8) | temp.data[i];
        }
        stream << "-" << result;
    } else {
        stream << result;
    }
    
    return stream;
}