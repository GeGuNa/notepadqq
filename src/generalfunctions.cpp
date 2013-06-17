/*
 *
 * This file is part of the Notepadqq text editor.
 *
 * Copyright(c) Notepadqq team.
 * http://notepadqq.sourceforge.net/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "generalfunctions.h"
#include <QStringList>
#include <QProcess>
#include <QTextDecoder>
#include <QFile>
#include <QDebug>

generalFunctions::generalFunctions()
{

}

QString generalFunctions::getFileMimeType(QString file)
{
    return getFileInformation(file, (MAGIC_ERROR|MAGIC_MIME_TYPE));
}

QString generalFunctions::getFileMimeEncoding(QString file)
{
    return getFileInformation(file, (MAGIC_ERROR|MAGIC_MIME_ENCODING));
}

QString generalFunctions::getFileType(QString file)
{
    return getFileInformation(file,(MAGIC_ERROR|MAGIC_RAW));
}

QString generalFunctions::getFileInformation(QString file, int flags)
{
    if((!(QFile(file).exists())) && (file == ""))return "";

    magic_t myt = magic_open(flags);
    magic_load(myt,NULL);
    QString finfo = magic_file(myt,file.toStdString().c_str());
    magic_close(myt);

    //We go a different route for checking encoding
    if((flags & MAGIC_MIME_ENCODING)) {
        //Don't ever return a codec we don't support, will cause crashes.
        foreach(QByteArray codec, QTextCodec::availableCodecs()){
            if(codec.toUpper() == finfo.toUpper()) {
                return codec;
            }
        }

        return "UTF-8";
    }

    return finfo;
}

QString generalFunctions::readDConfKey(QString schema, QString key)
{
    try {
      QProcess *process = new QProcess();
      QStringList *args = new QStringList();
      args->append("get");
      args->append(schema);
      args->append(key);
      process->start("gsettings", *args);
      if(process->waitForStarted(2000))
      {
          process->closeWriteChannel();
          process->waitForFinished(2000);
          QByteArray qba = process->readAll();
          QTextCodec *codec = QTextCodec::codecForLocale();
          QTextDecoder *decoder = codec->makeDecoder();
          QString result = decoder->toUnicode(qba);

          delete args;
          delete decoder;
          delete process;

          result = result.trimmed().mid(1, result.length()-3);
          return result;
      } else {
          return "";
      }
    }
    catch (...) {
       return "";
    }
}
