#include "Database.cpp"
#include <mysqlx/xdevapi.h>
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <cctype>

using namespace std;

// ================= UTILITY =================

void pulisciInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool stringaVuota(const string& s) {
    return s.find_first_not_of(" \t\n\r") == string::npos;
}

bool formatoDataValido(const string& data) {
    if (data.size() != 10) return false;
    if (data[4] != '-' || data[7] != '-') return false;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(data[i])) return false;
    }

    int mese = stoi(data.substr(5, 2));
    int giorno = stoi(data.substr(8, 2));

    return mese >= 1 && mese <= 12 && giorno >= 1 && giorno <= 31;
}

bool formatoMeseValido(const string& mese) {
    if (mese.size() != 7) return false;
    if (mese[4] != '-') return false;

    for (int i = 0; i < 7; i++) {
        if (i == 4) continue;
        if (!isdigit(mese[i])) return false;
    }

    int m = stoi(mese.substr(5, 2));
    return m >= 1 && m <= 12;
}

int trovaIdCategoria(mysqlx::Session& session, const string& nomeCategoria) {
    auto result = session.sql(
        "SELECT id FROM categorie WHERE name = ?"
    ).bind(nomeCategoria).execute();

    auto row = result.fetchOne();

    if (!row) {
        return -1;
    }

    return int(row[0]);
}

// ================= MODULO CATEGORIE =================

void gestioneCategorie(mysqlx::Session& session) {
    string nome;

    cout << "\n--- Gestione Categorie ---\n";
    cout << "Inserisci nome categoria: ";
    getline(cin, nome);

    if (stringaVuota(nome)) {
        cout << "Errore: il nome categoria non puo' essere vuoto.\n";
        return;
    }

    try {
        int id = trovaIdCategoria(session, nome);

        if (id != -1) {
            cout << "La categoria esiste gia'.\n";
            return;
        }

        session.sql(
            "INSERT INTO categorie(name) VALUES (?)"
        ).bind(nome).execute();

        cout << "Categoria inserita correttamente.\n";
    }
    catch (const mysqlx::Error& err) {
        cout << "Errore MySQL: " << err.what() << "\n";
    }
}

// ================= MODULO SPESE =================

void inserisciSpesa(mysqlx::Session& session) {
    string data;
    string categoria;
    string descrizione;
    double importo;

    cout << "\n--- Inserisci Spesa ---\n";

    cout << "Data spesa (YYYY-MM-DD): ";
    getline(cin, data);

    if (!formatoDataValido(data)) {
        cout << "Errore: formato data non valido.\n";
        return;
    }

    cout << "Importo: ";
    cin >> importo;

    if (cin.fail()) {
        pulisciInput();
        cout << "Errore: importo non valido.\n";
        return;
    }

    pulisciInput();

    if (importo <= 0) {
        cout << "Errore: l'importo deve essere maggiore di zero.\n";
        return;
    }

    cout << "Nome categoria: ";
    getline(cin, categoria);

    int idCategoria = trovaIdCategoria(session, categoria);

    if (idCategoria == -1) {
        cout << "Errore: la categoria non esiste.\n";
        return;
    }

    cout << "Descrizione facoltativa: ";
    getline(cin, descrizione);

    try {
        session.sql(
            "INSERT INTO spese(data_spesa, importo, id_categoria, descrizione) "
            "VALUES (?, ?, ?, ?)"
        ).bind(data, importo, idCategoria, descrizione).execute();

        cout << "Spesa inserita correttamente.\n";
    }
    catch (const mysqlx::Error& err) {
        cout << "Errore MySQL: " << err.what() << "\n";
    }
}

// ================= MODULO BUDGET =================

void definisciBudget(mysqlx::Session& session) {
    string mese;
    string categoria;
    double importo;

    cout << "\n--- Definisci Budget Mensile ---\n";

    cout << "Mese (YYYY-MM): ";
    getline(cin, mese);

    if (!formatoMeseValido(mese)) {
        cout << "Errore: formato mese non valido.\n";
        return;
    }

    cout << "Nome categoria: ";
    getline(cin, categoria);

    int idCategoria = trovaIdCategoria(session, categoria);

    if (idCategoria == -1) {
        cout << "Errore: la categoria non esiste.\n";
        return;
    }

    cout << "Importo budget: ";
    cin >> importo;

    if (cin.fail()) {
        pulisciInput();
        cout << "Errore: importo non valido.\n";
        return;
    }

    pulisciInput();

    if (importo <= 0) {
        cout << "Errore: il budget deve essere maggiore di zero.\n";
        return;
    }

    try {
        session.sql(
            "INSERT INTO budgets(mese, id_categoria, importo) "
            "VALUES (?, ?, ?) "
            "ON DUPLICATE KEY UPDATE importo = VALUES(importo)"
        ).bind(mese, idCategoria, importo).execute();

        cout << "Budget mensile salvato correttamente.\n";
    }
    catch (const mysqlx::Error& err) {
        cout << "Errore MySQL: " << err.what() << "\n";
    }
}

// ================= REPORT =================

void reportTotalePerCategoria(mysqlx::Session& session) {
    cout << "\n--- Totale Spese per Categoria ---\n";

    try {
        auto result = session.sql(
            "SELECT c.name, IFNULL(SUM(s.importo), 0) AS totale "
            "FROM categorie c "
            "LEFT JOIN spese s ON c.id = s.id_categoria "
            "GROUP BY c.id, c.name "
            "ORDER BY c.name"
        ).execute();

        cout << left << setw(25) << "Categoria" << "Totale Speso\n";
        cout << "----------------------------------------\n";

        for (auto row : result) {
            cout << left << setw(25) << string(row[0])
                << fixed << setprecision(2) << double(row[1]) << "\n";
        }
    }
    catch (const mysqlx::Error& err) {
        cout << "Errore MySQL in reportTotalePerCategoria: " << err.what() << "\n";
    }
}

void reportMensileVsBudget(mysqlx::Session& session) {
    cout << "\n--- Spese Mensili vs Budget ---\n";

  try {
    auto result = session.sql(
        "SELECT b.mese, c.name, b.importo AS budget, "
        "IFNULL(SUM(s.importo), 0) AS speso "
        "FROM budgets b "
        "JOIN categorie c ON b.id_categoria = c.id "
        "LEFT JOIN spese s ON s.id_categoria = b.id_categoria "
        "AND DATE_FORMAT(s.data_spesa, '%Y-%m') = b.mese "
        "GROUP BY b.mese, c.name, b.importo "
        "ORDER BY b.mese, c.name"
    ).execute();
    for (auto row : result) {
        string mese = string(row[0]);
        string categoria = string(row[1]);
        double budget = double(row[2]);
        double speso = double(row[3]);

        cout << "\nMese: " << mese << "\n";
        cout << "Categoria: " << categoria << "\n";
        cout << "Budget: " << fixed << setprecision(2) << budget << "\n";
        cout << "Speso: " << fixed << setprecision(2) << speso << "\n";

        if (speso > budget) {
            cout << "Stato: SUPERAMENTO BUDGET\n";
        }
        else {
            cout << "Stato: OK, BUDGET NON SUPERATO\n";
        }
    }
    // uso result...
}
catch (const mysqlx::Error &err) {
    std::cerr << "mysqlx::Error: " << err.what() << "\n";
    return; // o gestire l'errore
}
catch (const std::exception &e) {
    std::cerr << "std::exception: " << e.what() << "\n";
}


}

void reportElencoSpese(mysqlx::Session& session) {
    cout << "\n--- Elenco Completo Spese ---\n";
    auto result = session.sql(
        "SELECT DATE_FORMAT(s.data_spesa, '%Y/%m/%d'), c.name, s.importo, IFNULL(s.descrizione, '') "
        "FROM spese s "
        "JOIN categorie c ON s.id_categoria = c.id "
        "ORDER BY s.data_spesa ASC"
    ).execute();

    cout << left << setw(15) << "Data"
        << setw(20) << "Categoria"
        << setw(12) << "Importo"
        << "Descrizione\n";

    cout << "------------------------------------------------------------\n";

    for (auto row : result) {
        cout << left << setw(15) << string(row[0])
            << setw(20) << string(row[1])
            << setw(12) << fixed << setprecision(2) << double(row[2])
            << string(row[3]) << "\n";
    }
}

void menuReport(mysqlx::Session& session) {
    session.sql(
        "USE gestione_spesa").execute();
    int scelta;

    do {
        cout << "\n--- Menu Report ---\n";
        cout << "1. Totale spese per categoria\n";
        cout << "2. Spese mensili vs budget\n";
        cout << "3. Elenco completo delle spese ordinate per data\n";
        cout << "4. Ritorna al menu principale\n";
        cout << "Inserisci scelta: ";

        cin >> scelta;

        if (cin.fail()) {
            pulisciInput();
            cout << "Scelta non valida. Riprovare.\n";
            continue;
        }

        pulisciInput();

        switch (scelta) {
        case 1:
            reportTotalePerCategoria(session);
            break;
        case 2:
            reportMensileVsBudget(session);
            break;
        case 3:
            reportElencoSpese(session);
            break;
        case 4:
            cout << "Ritorno al menu principale.\n";
            break;
        default:
            cout << "Scelta non valida. Riprovare.\n";
        }

    } while (scelta != 4);
}

// ================= MAIN =================

int main() {
    cout << "Benvenuto nel Sistema di Gestione delle Spese Personali e del Budget!\n";

    Database db(
        DatabaseConfig::HOST,
        DatabaseConfig::PORT,
        DatabaseConfig::USER,
        DatabaseConfig::PASSWORD,
        DatabaseConfig::DATABASE
    );

    if (!db.connect()) {
        cout << "Impossibile avviare il programma.\n";
        return 1;
    }

    mysqlx::Session& session = db.getSession();
    session.sql(
        "USE gestione_spesa").execute();
    int scelta;

    do {
        cout << "\n-------------------------\n";
        cout << "SISTEMA SPESE PERSONALI\n";
        cout << "-------------------------\n";
        cout << "1. Gestione Categorie\n";
        cout << "2. Inserisci Spesa\n";
        cout << "3. Definisci Budget Mensile\n";
        cout << "4. Visualizza Report\n";
        cout << "5. Esci\n";
        cout << "-------------------------\n";
        cout << "Inserisci la tua scelta: ";

        cin >> scelta;

        if (cin.fail()) {
            pulisciInput();
            cout << "Scelta non valida. Riprovare.\n";
            continue;
        }

        pulisciInput();

        switch (scelta) {
        case 1:
            gestioneCategorie(session);
            break;
        case 2:
            inserisciSpesa(session);
            break;
        case 3:
            definisciBudget(session);
            break;
        case 4:
            menuReport(session);
            break;
        case 5:
            cout << "Uscita dal programma.\n";
            break;
        default:
            cout << "Scelta non valida. Riprovare.\n";
        }

    } while (scelta != 5);

    return 0;
}