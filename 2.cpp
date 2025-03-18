#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

class Player { //Добавляем класс Player с полями number и name для хранения информации о каждом игроке в хэш таблице
public:
    int number;
    std::string name;

    Player(int num, const std::string& n) : number(num), name(n) {}
};

template <typename KeyType, typename ValueType>
class HashTable { //Класс для реализации хэш-таблицы 
private:
    struct HashNode { //Эта структура представляет узел хэш-таблицы, содержащий ключ и хначение(игрока)
        KeyType key;
        ValueType value;
    };

    std::vector<HashNode*> table;
    int capacity;
    int size;

    int hashFunction(const KeyType& key) const { //Вычисляет хэш-значение для заданного ключа и возвращает индекс в массиве, где хранится соответствующее значение
        return std::hash<KeyType>()(key) % capacity;
    }

    int quadraticProbe(int hash, int i) const { //Реализует квадратичное пробирование для разрешения коллизий
        return (hash + i * i) % capacity;
    }

public:
    HashTable(int initialCapacity) : capacity(initialCapacity), size(0) {
        table.resize(capacity, nullptr);
    }

    ~HashTable() {
        for (auto node : table) {
            delete node;
        }
    }

    void insert(const KeyType& key, const ValueType& value) { //Добавляет новую пару ключ-значение в хэш-таблицу. Если таблица заполняется более, чем на половину, вызывается метод resize для увеличения ее размера
        if (size >= capacity / 2) {
            resize();
        }

        int hash = hashFunction(key);
        for (int i = 0; i < capacity; ++i) {
            int index = quadraticProbe(hash, i);
            if (table[index] == nullptr || table[index]->key == key) {
                if (table[index] == nullptr) {
                    size++;
                }
                table[index] = new HashNode{key, value};
                return;
            }
        }
        throw std::overflow_error("HashTable is full");
    }

    bool find(const KeyType& key, ValueType& value) const { //Ищет значение по заданному ключу и возвращает true, если оно найдено, и false, если нет
        int hash = hashFunction(key);
        for (int i = 0; i < capacity; ++i) {
            int index = quadraticProbe(hash, i);
            if (table[index] == nullptr) {
                return false; // Key not found
            }
            if (table[index]->key == key) {
                value = table[index]->value;
                return true; // Key found
            }
        }
        return false; // Key not found
    }

    void remove(const KeyType& key) { //Удаляет узел с заданным ключом из хэш-таблицы. Если узел найден, он освобождает память и уменьшает размер таблицы
        int hash = hashFunction(key);
        for (int i = 0; i < capacity; ++i) {
            int index = quadraticProbe(hash, i);
            if (table[index] == nullptr) {
                return; // Key not found
            }
            if (table[index]->key == key) {
                delete table[index];
                table[index] = nullptr;
                size--;
                return;
            }
        }
    }

    void resize() { //Увеличивает размер хэш-таблицы и перераспределяет существующие узлы в новую таблицу с удвоенной емкостью. Это помогает избежать переполнения и улучшает производительность при большом количестве элементов
        int oldCapacity = capacity;
        capacity *= 2;
        std::vector<HashNode*> oldTable = table;

        table.clear();
        table.resize(capacity, nullptr);
        size = 0;

        for (auto node : oldTable) {
            if (node != nullptr) {
                insert(node->key, node->value);
                delete node;
            }
        }
    }

    void print() const { //Выводит содержимое хэш-таблицы, показывая индексы и соответствующие им пары ключ-значение
        std::cout << "HashTable contents:\n";
        for (int i = 0; i < capacity; ++i) {
            if (table[i] != nullptr) {
                std::cout << "Index " << i << ": "
                          << "Key: " << table[i]->key 
                          << ", Player: " << table[i]->value.name 
                          << " (Number: " << table[i]->value.number << ")\n";
            } else {
                std::cout << "Index " << i << ": [empty]\n";
            }
        }
    }

    int getSize() const {
        return size;
    }
};

int main() {
    HashTable<int, Player> hashTable(10);

    hashTable.insert(1, Player(1, "Маша"));
    hashTable.insert(2, Player(2, "Саша"));
    hashTable.insert(3, Player(3, "Паша"));

    hashTable.print(); // Вывод содержимого хэш-таблицы

    Player foundPlayer(0, "");
    if (hashTable.find(2, foundPlayer)) {
        std::cout << "Found player: " << foundPlayer.name << " with number: " << foundPlayer.number << std::endl;
    } else {
        std::cout << "Player not found." << std::endl;
    }

    hashTable.remove(2);
    
    hashTable.print(); // Вывод содержимого хэш-таблицы после удаления

    if (hashTable.find(2, foundPlayer)) {
        std::cout << "Found player: " << foundPlayer.name << " with number: " << foundPlayer.number << std::endl;
    } else {
        std::cout << "Player not found." << std::endl;
    }

    return 0;
}

