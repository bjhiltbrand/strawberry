/*
 * Strawberry Music Player
 * Copyright 2018-2021, Jonas Kvinge <jonas@jkvinge.net>
 *
 * Strawberry is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Strawberry is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Strawberry.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LYRICSFETCHER_H
#define LYRICSFETCHER_H

#include "config.h"

#include <QtGlobal>
#include <QObject>
#include <QMetaType>
#include <QQueue>
#include <QSet>
#include <QList>
#include <QHash>
#include <QString>
#include <QUrl>

class QTimer;
class LyricsProviders;
class LyricsFetcherSearch;

struct LyricsSearchRequest {
  explicit LyricsSearchRequest() : id(0) {}
  quint64 id;
  QString artist;
  QString album;
  QString title;
};

struct LyricsSearchResult {
  explicit LyricsSearchResult() : score(0.0) {}
  QString provider;
  QString artist;
  QString album;
  QString title;
  QString lyrics;
  float score;
};
using LyricsSearchResults = QList<LyricsSearchResult>;

Q_DECLARE_METATYPE(LyricsSearchResult)
Q_DECLARE_METATYPE(QList<LyricsSearchResult>)

class LyricsFetcher : public QObject {
  Q_OBJECT

 public:
  explicit LyricsFetcher(LyricsProviders *lyrics_providers, QObject *parent = nullptr);
  ~LyricsFetcher() override {}

  quint64 Search(const QString &artist, const QString &album, const QString &title);
  void Clear();

 private:
  void AddRequest(const LyricsSearchRequest &req);

 signals:
  void LyricsFetched(quint64 request_id, QString provider, QString lyrics);
  void SearchFinished(quint64 request_id, LyricsSearchResults results);

 private slots:
  void SingleSearchFinished(const quint64 request_id, const LyricsSearchResults &results);
  void SingleLyricsFetched(const quint64 request_id, const QString &provider, const QString &lyrics);
  void StartRequests();

 private:
  static const int kMaxConcurrentRequests;

  LyricsProviders *lyrics_providers_;
  quint64 next_id_;

  QQueue<LyricsSearchRequest> queued_requests_;
  QHash<quint64, LyricsFetcherSearch*> active_requests_;

  QTimer *request_starter_;

};

#endif  // LYRICSFETCHER_H
