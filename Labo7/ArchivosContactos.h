#pragma once
#include <QString>

class ArchivoContactos
{
public:
    QString ruta() const;
    bool guardar(const QString& nombre,
        const QString& telefono,
        const QString& correo) const;
};

