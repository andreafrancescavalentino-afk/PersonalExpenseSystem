CREATE DATABASE IF NOT EXISTS gestione_spesa;
USE gestione_spesa;

CREATE TABLE IF NOT EXISTS categorie (
    id INT AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    CONSTRAINT pk_categorie PRIMARY KEY (id),
    CONSTRAINT uq_categorie_name UNIQUE (name),
    CONSTRAINT chk_categorie_name CHECK (CHAR_LENGTH(TRIM(name)) > 0)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS spese (
    id INT AUTO_INCREMENT,
    data_spesa DATE NOT NULL,
    importo DECIMAL(10,2) NOT NULL,
    id_categoria INT NOT NULL,
    descrizione VARCHAR(255),
    CONSTRAINT pk_spesa PRIMARY KEY (id),
    CONSTRAINT fk_spesa_categorie FOREIGN KEY (id_categoria)
        REFERENCES categorie(id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT,
    CONSTRAINT chk_spesa_importo CHECK (importo > 0)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS budgets (
    id INT AUTO_INCREMENT,
    mese CHAR(7) NOT NULL,
    id_categoria INT NOT NULL,
    importo DECIMAL(10,2) NOT NULL,
    CONSTRAINT pk_budgets PRIMARY KEY (id),
    CONSTRAINT fk_budgets_categorie FOREIGN KEY (id_categoria)
        REFERENCES categorie(id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT,
    CONSTRAINT uq_budgets_mese_categoria UNIQUE (mese, id_categoria),
    CONSTRAINT chk_budgets_importo CHECK (importo > 0)
) ENGINE=InnoDB;