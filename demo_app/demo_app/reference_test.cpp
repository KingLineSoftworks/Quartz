#include <functional>
#include <iostream>

class Client {
public:
    Client() : mr_a(default_a) {}

    const uint32_t& get_a() const { return mr_a; }

    void set_a(uint32_t& a) {
        const uint32_t old_a = mr_a;
        mr_a = a;
        std::cout << "Updating mr_a from " << old_a << " to " << mr_a << std::endl << std::endl;
    }

public:
    static uint32_t default_a;

public:
    std::reference_wrapper<uint32_t> mr_a;
};

class Owner {
public:
    Owner(const uint32_t a) : m_a(a) {}

    uint32_t get_a() const { return m_a; }

    void set_client_reference(Client* p_client) {
        p_client->set_a(m_a);
    }

    void update() {
        const uint32_t old_a = m_a;
        m_a += 1;
        std::cout << "m_a updated from " << old_a << " to " << m_a << std::endl;
    }

public:
    uint32_t m_a;
};

uint32_t Client::default_a = 42;

int main() {
    Client client;
    Owner owner(100);

    owner.set_client_reference(&client);

    for (uint32_t i = 0; i < 3; ++i) {
        owner.update();
        std::cout << "  owner  a: " << owner.get_a() << std::endl;
        std::cout << "  client a: " << client.get_a() << std::endl << std::endl;
    }
    
    return 0;
}

