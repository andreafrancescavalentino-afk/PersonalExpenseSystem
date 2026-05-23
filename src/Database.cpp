#pragma once

#include <mysqlx/xdevapi.h>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <utility>

namespace DatabaseConfig {
    inline const std::string HOST = "localhost";
    inline const int PORT = 33060;
    inline const std::string USER = "root";
    inline const std::string PASSWORD = "password"; // cambia qui
    inline const std::string DATABASE = "gestione_spesa";
}

class Database {
private:
    std::string host;
    int port;
    std::string user;
    std::string password;
    std::string schema;

    std::unique_ptr<mysqlx::Session> session;

    void executeSqlFile(const std::string& fileName) {
        std::ifstream file(fileName);

        if (!file.is_open()) {
            throw std::runtime_error("Impossibile aprire il file SQL: " + fileName);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        std::string sql = buffer.str();
        std::string statement;

        for (char c : sql) {
            statement += c;

            if (c == ';') {
                if (statement.find_first_not_of(" \n\r\t;") != std::string::npos) {
                    session->sql(statement).execute();
                }
                statement.clear();
            }
        }
    }

   // void createSchemaAndTables() {
        //executeSqlFile("SQL/schema.sql");
        //executeSqlFile("SQL/sample_data.sql");
    //}

public:
    Database(std::string host, int port, std::string user, std::string password, std::string schema)
        : host(std::move(host)),
        port(port),
        user(std::move(user)),
        password(std::move(password)),
        schema(std::move(schema)) {
    }

    bool connect() {
        try {
            session = std::make_unique<mysqlx::Session>(
                mysqlx::SessionOption::HOST, host.c_str(),
                mysqlx::SessionOption::PORT, port,
                mysqlx::SessionOption::USER, user.c_str(),
                mysqlx::SessionOption::PWD, password.c_str()
            );

          //  createSchemaAndTables();

            std::cout << "Connessione MySQL riuscita. Database pronto.\n";
            return true;
        }
        catch (const mysqlx::Error& err) {
            std::cerr << "Errore MySQL: " << err.what() << "\n";
            return false;
        }
        catch (const std::exception& ex) {
            std::cerr << "Errore generico: " << ex.what() << "\n";
            return false;
        }
    }

    mysqlx::Session& getSession() {
        if (!session) {
            throw std::runtime_error("Sessione MySQL non inizializzata.");
        }
        return *session;
    }
};