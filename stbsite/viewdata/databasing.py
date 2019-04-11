import sqlalchemy
import subprocess


def listTables(dbname):
    engine = sqlalchemy.create_engine('sqlite:///'+dbname, echo=False)
    return engine.table_names()


def dropTable(dbname, table):
    engine = sqlalchemy.create_engine('sqlite:///'+dbname, echo=False)
    engine.execute("DROP TABLE IF EXISTS '%s'" % table)


def viewTable(dbname, table):
    engine = sqlalchemy.create_engine('sqlite:///'+dbname, echo=False)
    return engine.execute("SELECT * FROM '%s'" % table).fetchall()


if __name__ == '__main__':
        for i in listTables('data/db.sqlite'):
            dropTable('data/db.sqlite', i)
        subprocess.call(['rm', '-rf', 'data/*.dat'])
