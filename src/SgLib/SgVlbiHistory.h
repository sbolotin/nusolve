/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2010-2020 Sergei Bolotin.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SG_VLBI_HISTORY_H
#define SG_VLBI_HISTORY_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <QtCore/QString>
#include <QtCore/QList>


#include <SgMJD.h>


class SgDbhImage;

/***===================================================================================================*/
/**
 * SgVlbiHistoryRecord, a record of history block of DBH files.
 *
 */
/**====================================================================================================*/
class SgVlbiHistoryRecord
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgVlbiHistoryRecord();

  /**A constructor.
   * Creates an object with parameters.
   */
  inline SgVlbiHistoryRecord(const SgMJD&, int, const QString&, bool=false);

  inline SgVlbiHistoryRecord(const SgVlbiHistoryRecord& rec);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgVlbiHistoryRecord() {};


  //
  // Interfaces:
  //
  // gets:
  /**Returns epoch.
   */
  inline const SgMJD& getEpoch() const;

  /**Returns version.
   */
  inline int getVersion() const;

  /**Returns text of the recotrd.
   */
  inline const QString& getText() const;

  /**Returns "editable" status.
   */
  inline bool getIsEditable() const;

  // sets:
  /**Sets up epoch of the record.
   * \param t -- the new epoch;
   */
  inline void setEpoch(const SgMJD& t);

  /**Sets up version where the record was added.
   * \param t -- the new epoch;
   */
  inline void setVersion(int v);

  /**Sets up a content of the record.
   * \param text -- a historical text;
   */
  inline void setText(const QString& text);

  /**Sets up "editable" status.
   * \param is -- status;
   */
  inline void setIsEditable(bool is);

  //
  // Functions:
  //
  inline const QString className() const;

private:
  //
  SgMJD                         epoch_;       // an epoch of the history text band
  int                           version_;     // version of the file where it was added
  QString                       text_;        // a text of a historical record
  bool                          isEditable_;  // user (an owner) can edit or delete the record
};
/*=====================================================================================================*/



/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgDbhHistoryEntry;
class SgVlbiHistory : public QList<SgVlbiHistoryRecord*>
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgVlbiHistory();

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgVlbiHistory();

  //
  // Interfaces:
  //
  // gets:
  inline const QString& getFirstRecordFromUser() const;

  // sets:
  inline void setFirstRecordFromUser(const QString&);


  //
  // Functions:
  //
  void importDbhHistoryBlock(SgDbhImage*);

  void export2DbhHistoryBlock(SgDbhImage*);
  
//  void addHistoryRecord(const QString&, const SgMJD& t = SgMJD::currentMJD().toUtc());
  void addHistoryRecord(const QString&, const SgMJD& t);

  inline const QString className() const;


private:
  // for David:
  QString                       firstRecordFromUser_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgVlbiHistoryEntry inline members:                                                                  */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgVlbiHistoryRecord::SgVlbiHistoryRecord() :
  epoch_(tZero),
  version_(-1),
  text_("Unnatural history record"),
  isEditable_(false)
{
};


//
inline SgVlbiHistoryRecord::SgVlbiHistoryRecord(const SgMJD& t, int v, const QString& text, bool is) :
  epoch_(t),
  version_(v),
  text_(text),
  isEditable_(is)
{
};



//
inline SgVlbiHistoryRecord::SgVlbiHistoryRecord(const SgVlbiHistoryRecord& rec) :
  epoch_(rec.getEpoch()),
  version_(rec.getVersion()),
  text_(rec.getText()),
  isEditable_(rec.getIsEditable())
{
};



//
// INTERFACE:
//
//
inline const SgMJD& SgVlbiHistoryRecord::getEpoch() const
{
  return epoch_;
};



//
inline int SgVlbiHistoryRecord::getVersion() const
{
  return version_;
};



//
inline const QString& SgVlbiHistoryRecord::getText() const
{
  return text_;
};



//
inline bool SgVlbiHistoryRecord::getIsEditable() const
{
  return isEditable_;
};



//
inline void SgVlbiHistoryRecord::setEpoch(const SgMJD& t)
{
  epoch_ = t;
};



//
inline void SgVlbiHistoryRecord::setVersion(int v)
{
  version_ = v;
};



//
inline void SgVlbiHistoryRecord::setText(const QString& text)
{
  text_ = text;
};



//
inline void SgVlbiHistoryRecord::setIsEditable(bool is)
{
  isEditable_ = is;
};



//
inline const QString SgVlbiHistoryRecord::className() const
{
  return "SgVlbiHistoryRecord";
};
/*=====================================================================================================*/






/*================================================================================================*/
/*                                                                                                */
/* SgVlbiHistory inline members:                                                                  */
/*                                                                                                */
/*================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgVlbiHistory::SgVlbiHistory() :
  QList<SgVlbiHistoryRecord*>(),
  firstRecordFromUser_()
{
};



//
inline SgVlbiHistory::~SgVlbiHistory()
{
  for (int i=0; i<size(); i++)
    delete at(i);
  clear();
};



//
inline const QString SgVlbiHistory::className() const
{
  return "SgVlbiHistory";
};



//
inline const QString& SgVlbiHistory::getFirstRecordFromUser() const
{
  return firstRecordFromUser_;
};



//
inline void SgVlbiHistory::setFirstRecordFromUser(const QString& str)
{
  firstRecordFromUser_ = str;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
#endif // SG_VLBI_HISTORY_H
