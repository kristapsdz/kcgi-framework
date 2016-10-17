PRAGMA journal_mode=WAL;
PRAGMA foreign_keys=ON;

-- A user in the system.

CREATE TABLE user (
	-- E-mail address (and login identifier). 
	email TEXT NOT NULL,
	-- Hash representation of password.
	hash TEXT NOT NULL,
	-- Unique identifier.
	id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
	unique (email)
);

-- An authenticated session.

CREATE TABLE sess (
	-- User attached to session.
	userid INTEGER NOT NULL,
	-- Unique cookie generated for session.
	token INTEGER NOT NULL,
	-- Unique identifier.
	id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
	FOREIGN KEY(userid) REFERENCES user(id) ON DELETE CASCADE
);
