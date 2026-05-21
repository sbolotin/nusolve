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

#include <iostream>
#include <stdlib.h>


#include <SgDbhImage.h>
#include <SgDbhFormat.h>
#include <SgLogger.h>
#include <SgVersion.h>



const QString SgDbhDatumDescriptor::typeNames_[6] = {"R8", "I2", "A2", "D8", "J4", "??"};


/*=======================================================================================================
*
*                      SgDbhDatumDescriptor, methods:
* 
*======================================================================================================*/
SgDbhDatumDescriptor::SgDbhDatumDescriptor()
{
  setLCode("ARTIFIC."); 
  setDescription("An undescribed item");
  dim1_ = dim2_ = dim3_ = 0;
  modifiedAtVersion_ = nTc_ = nTe_ = -1;
  offset_ = -1; 
  type_ = T_UNKN;
};



//
void SgDbhDatumDescriptor::setLCode(const QString& lCode)
{
  lCode_ = lCode.leftJustified(8, ' ', true);
};



//
void SgDbhDatumDescriptor::setDescription(const QString& description)
{
  description_ = description.leftJustified(32, '.', true);
};



//
SgDbhDatumDescriptor::SgDbhDatumDescriptor(const QString& lCode, const QString& description)
{
  setLCode(lCode);
  setDescription(description);
  dim1_ = dim2_ = dim3_ = 0; 
  modifiedAtVersion_ = nTc_ = nTe_ = -1; 
  offset_ = -1; 
  type_ = T_UNKN;
};



//
SgDbhDatumDescriptor& SgDbhDatumDescriptor::operator=(const SgDbhDatumDescriptor& descriptor)
{
  setLCode(descriptor.lCode_); 
  setDescription(descriptor.description_);
  type_ = descriptor.type_; 
  dim1_ = descriptor.dim1_; 
  dim2_ = descriptor.dim2_;
  dim3_ = descriptor.dim3_; 
  modifiedAtVersion_ = descriptor.modifiedAtVersion_; 
  nTc_ = descriptor.nTc_; 
  nTe_ = descriptor.nTe_;
  offset_ = descriptor.offset_; 
  return *this;
};
/*=====================================================================================================*/




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
SgDbhImage::SgDbhImage() :
  canonicalName_(""),
  history_(),
  descriptorByLCode_(),
  listOfDataBlocksToc0_(),
  listOfObservations_(),
  listOfNewDescriptors_(),
  listOfDeletedDescriptors_(),
  alterCode_("@")
{
  contentState_ = CS_Bare;
  formatState_ = FS_Bare;
  currentVersion_ = -1;
  dumpStream_ = NULL;
  startBlock_ = new SgDbhStartBlock;
  format_ = new SgDbhFormat;
  // test purposes:
  isSessionCodeAltered_ = false;
};



//
SgDbhImage::~SgDbhImage()
{
  // clear history list:
  clearHistoryList();
  // just clear the hash
  descriptorByLCode_.clear();
  //
  while (!listOfDataBlocksToc0_.isEmpty())
    delete listOfDataBlocksToc0_.takeFirst();
  //
  // clear list of observations:
  clearListOfObservations();
  //
  while (!listOfNewDescriptors_.isEmpty())
    delete listOfNewDescriptors_.takeFirst();
  //
  while (!listOfDeletedDescriptors_.isEmpty())
    delete listOfDeletedDescriptors_.takeFirst();
  //
  delete startBlock_;
  startBlock_ = NULL;
  //
  delete format_;
  format_ = NULL;

  dumpStream_ = NULL;
};



//
SgMJD SgDbhImage::fileCreationEpoch() const
{
  return startBlock_->epoch();
};



//
int SgDbhImage::fileVersion() const
{
  return startBlock_->version();
};



//
const QString& SgDbhImage::fileName() const
{
  return startBlock_->dbName();
};



//
const QString& SgDbhImage::sessionDescription() const
{
  return startBlock_->expDescript();
};



//
const QString& SgDbhImage::sessionID() const
{
  return startBlock_->sessionID();
};



//
const QString& SgDbhImage::previousFileName() const
{
  return startBlock_->prevDb();
};



//
const QString& SgDbhImage::previousFileDescription() const
{
  return startBlock_->prevDescript();
};



//
void SgDbhImage::clearHistoryList()
{
  // clear history list:
  while (!history_.isEmpty())
    delete history_.takeFirst();
};



//
void SgDbhImage::clearListOfObservations()
{
  // clear list of observations:
  while (!listOfObservations_.isEmpty())
    delete listOfObservations_.takeFirst();
};



//
void SgDbhImage::addHistoryEntry(const QString& text, const SgMJD& t)
{
  SgDbhHistoryEntry *entry = new SgDbhHistoryEntry;
  entry->setEvent(text, startBlock_->dbName(),  currentVersion_, t);
  history_ << entry;
  contentState_ = CS_DataAltered;
};



//
SgDbhPhysicalRecord *SgDbhImage::properRecord(SgDbhDatumDescriptor* descriptor, int obsNum)
{
  SgDbhDataBlock *dataBlock=NULL;
  
  if (descriptor->nTc()==0) //TOC#0:
  {
    if (obsNum>=0) // just notify user:
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        QString().sprintf(": properRecord(): obsNum %d is greater or equal 0 for TOC#0",
        obsNum));
    dataBlock = listOfDataBlocksToc0_.at(descriptor->nTe());
  }
  else // Observations:
  {
    if (obsNum<0)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        QString().sprintf(": properRecord(): obsNum %d is less than 0",
        obsNum));
      return NULL;
    };
    if (listOfObservations_.size()<=obsNum)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        QString().sprintf(": properRecord(): obsNum %d >= listOfObservations_.size(), %d",
        obsNum, listOfObservations_.size()));
      return NULL;
    };
    // here ranges of obsNum are OK
    SgDbhObservationEntry* obsEntry = listOfObservations_.at(obsNum);
    if (!obsEntry)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        QString().sprintf(": properRecord(): Observation entry is NULL for obsNum %d",
        obsNum));
      return NULL;
    };
    QList<SgDbhDataBlock*> *listOfDataBlocks = obsEntry->dataBlocksFromTocI(descriptor->nTc());
    if (!listOfDataBlocks)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        QString().sprintf(": properRecord(): listOfDataBlocks is NULL for obsNum %d and TOC#%d",
        obsNum, descriptor->nTc()));
      return NULL;
    };
    dataBlock = listOfDataBlocks->at(descriptor->nTe());
  };

  if (!dataBlock)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      QString().sprintf(": properRecord(): Data Block is NULL for obsNum %d TOC#%d TE#%d",
      obsNum, descriptor->nTc(), descriptor->nTe()));
    return NULL;
  };
  QHash<int, SgDbhPhysicalRecord*> *recByType = dataBlock->recordByType();
  if (!recByType->contains(descriptor->type()))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      QString().sprintf(": properRecord(): recordByType_ hash (TOC#%d, TE#%d) does not contain"
      "the record of type %d", descriptor->nTc(), descriptor->nTe(), descriptor->type()));
    return NULL;
  };
  return recByType->value(descriptor->type());
};



//
QString SgDbhImage::getStr(SgDbhDatumDescriptor *d, int i, int j, int obsNumber)
{
  if (!d)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() + 
      ": getStr(): descriptor is NULL");
    return QString("UNDEF");
  };
  return ((SgDbhDataRecordString*)properRecord(d, obsNumber))->getValue(d, i,j);
};



//
template<class C> C SgDbhImage::getData(const QString& typeName, 
  SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber)
{
  if (!d)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() + 
      ": getData(): the " + typeName + " descriptor is NULL");
    return C (0);
  };
  return ((SgDbhDataRecord<C>*)properRecord(d, obsNumber))->value(d, i,j,k);
};



//
double SgDbhImage::getR8(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber)
{
  return getData<double>("double:R8", d, i,j,k, obsNumber);
};



//
double SgDbhImage::getD8(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber)
{
  return getData<double>("double:D8", d, i,j,k, obsNumber);
};



//
int SgDbhImage::getJ4(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber)
{
  return getData<int>("int:J4", d, i,j,k, obsNumber);
};



//
short SgDbhImage::getI2(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber)
{
  return getData<short>("short:I2", d, i,j,k, obsNumber);
};



//
void SgDbhImage::setStr(SgDbhDatumDescriptor *d, int i, int j, int obsNumber, const QString& str)
{
  if (contentState_!=CS_Bare && formatState_!=FS_Bare && formatState_!=FS_FormatAltering)
  {
    if (!d)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() + 
        ": setStr(): descriptor is NULL");
      return;
    };
    ((SgDbhDataRecordString*)properRecord(d, obsNumber))->setValue(d, i,j,  str);
    d->setModifiedAtVersion(currentVersion_);
    contentState_ = CS_DataAltered;
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() + 
      ": setStr(): the image is in inconsistent state");
};



//
template<class C> 
void SgDbhImage::setData(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber, C v)
{
  if (contentState_!=CS_Bare && formatState_!=FS_Bare && formatState_!=FS_FormatAltering)
  {
    if (!d)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() + 
        ": setData(): descriptor is NULL");
      return;
    };
    ((SgDbhDataRecord<C>*)properRecord(d, obsNumber))->access(d, i,j,k) = v;
    d->setModifiedAtVersion(currentVersion_);
    contentState_ = CS_DataAltered;
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() + 
      ": setData(): the image is in inconsistent state");
};



//
void SgDbhImage::setR8(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber, double r8)
{
  return setData<double>(d, i,j,k, obsNumber, r8);
};



//
void SgDbhImage::setD8(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber, double d8)
{
  return setData<double>(d, i,j,k, obsNumber, d8);
};



//
void SgDbhImage::setJ4(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber, int j4)
{
  return setData<int>(d, i,j,k, obsNumber, j4);
};



//
void SgDbhImage::setI2(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber, short i2)
{
  return setData<short>(d, i,j,k, obsNumber, i2);
};



//
void SgDbhImage::copyToc0Content()
{
  // check currently read TOC number:
  if (format_->currentTcNumber()!=0)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      QString().sprintf(": copyToc0Content(): Wrong order of calling, current TOC is TOC#%d",
      format_->currentTcNumber()));
    // anyway, we ask correct TOC number explicitly, so "return" is commented out
    /* return; */
  };
  
  // copy header (TOC#0) data to listOfDataBlocksToc0_:
  SgDbhTcBlock                 *tcBlock0=format_->listOfTcBlocks()->at(0);
  for (int teCount=0; teCount<tcBlock0->listOfTeBlocks()->size(); teCount++)
  {
    SgDbhDataBlock             *dataBlock=new SgDbhDataBlock; 
    *dataBlock = *tcBlock0->listOfTeBlocks()->at(teCount);
    listOfDataBlocksToc0_.append(dataBlock);
  };
  //
  // collect all parameter descriptors to descriptorByLCode_:
  for (int i=0; i<format_->listOfTcBlocks()->size(); i++)
  {
    SgDbhTcBlock               *tcBlock=format_->listOfTcBlocks()->at(i);
    for (int j=0; j<tcBlock->listOfTeBlocks()->size(); j++)
    {
      SgDbhTeBlock             *teBlock=tcBlock->listOfTeBlocks()->at(j);
      for (int k=0; k<teBlock->listOfDescriptors()->size(); k++)
      {
        SgDbhDatumDescriptor   *descriptor=teBlock->listOfDescriptors()->at(k);
        // check for duplicates (could happen for old files):
        if (descriptorByLCode_.contains(descriptor->getLCode()))
          // complain:
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            QString("").sprintf(": copyToc0Content(): the TOC#0 already contains the parameter [%s]",
            qPrintable(descriptor->getLCode())));
        else
          if (descriptor->getLCode()!="R-FILLER" && 
              descriptor->getLCode()!="I-FILLER" &&
              descriptor->getLCode()!="A-FILLER" &&
              descriptor->getLCode()!="D-FILLER" &&
              descriptor->getLCode()!="J-FILLER")
            descriptorByLCode_.insert(descriptor->getLCode(), descriptor);
      };
    };
  };
};



//
void SgDbhImage::startFormatModifying()
{
  formatState_ = FS_FormatAltering;


};



//
void SgDbhImage::finisFormatModifying()
{
  // check lists:
  if (listOfNewDescriptors_.isEmpty() && listOfDeletedDescriptors_.isEmpty()) // nothing to do:
  {
    formatState_ = FS_FormatFromFile;
    return;
  };
  for (int i=0; i<listOfNewDescriptors_.size(); i++)
    for (int j=0; j<listOfDeletedDescriptors_.size() && i<listOfNewDescriptors_.size(); j++)
    {
      SgDbhDatumDescriptor  *d2Add = listOfNewDescriptors_.at(i);
      SgDbhDatumDescriptor  *d2Del = listOfDeletedDescriptors_.at(j);
      if (d2Add->getLCode() == d2Del->getLCode())
      {
        listOfNewDescriptors_.removeAt(i);
        listOfDeletedDescriptors_.removeAt(j);
        i--;
        j--;
        delete d2Add;
        delete d2Del;
      };
    };

  int           numOfModified=0;
  
  // make arrangements:
  // first, remove parameters to be deleted from the lists and the hashes:
  while (!listOfDeletedDescriptors_.isEmpty())    // parameters to delete:
  {
    SgDbhDatumDescriptor  *d = listOfDeletedDescriptors_.takeFirst();
    for (int tcIdx=0; tcIdx<format_->listOfTcBlocks()->size(); tcIdx++)
    {
      SgDbhTcBlock *tcBlock = format_->listOfTcBlocks()->at(tcIdx);
      for (int teIdx=0; teIdx<tcBlock->listOfTeBlocks()->size(); teIdx++)
      {
        SgDbhTeBlock *teBlock = tcBlock->listOfTeBlocks()->at(teIdx);
        for (int i=0; i<teBlock->listOfDescriptors()->size(); i++)
        {
          SgDbhDatumDescriptor* descriptor=teBlock->listOfDescriptors()->at(i);
          if (descriptor->getLCode() == d->getLCode() && (d->nTc()<0 || d->nTc()==tcIdx)) // got it
          {
            // remove pointers from two hashes and the list:
            tcBlock->descriptorByLCode()->remove(descriptor->getLCode());
            descriptorByLCode_.remove(descriptor->getLCode());
            teBlock->listOfDescriptors()->removeAt(i);
            teBlock->setIsFormatModified(true);
            // delete the descriptor
            delete descriptor;
            i--; // and adjust counter
            numOfModified++;
          };
        };
      };
    };
    delete d;
  };
  logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
    QString().sprintf(": finisFormatModifying(): deleted %d descriptors",
    numOfModified));
  
  numOfModified = 0;
  // then, add new parameters to the lists and the hashes:
  while (!listOfNewDescriptors_.isEmpty())        // new parameters:
  {
    SgDbhDatumDescriptor  *d = listOfNewDescriptors_.takeFirst();
    SgDbhTcBlock *tcBlock = format_->listOfTcBlocks()->at(d->nTc());
    SgDbhTeBlock *teBlock = tcBlock->listOfTeBlocks()->at(d->nTe());
    
    SgDbhDatumDescriptor* descriptor = new SgDbhDatumDescriptor;
    *descriptor = *d;
    int i = 0;
    // find first record with this type,
    while (i < teBlock->listOfDescriptors()->size() && 
          descriptor->type() != teBlock->listOfDescriptors()->at(i)->type())
      i++;
    // then move to the last record with this type,
    while (i < teBlock->listOfDescriptors()->size() && 
          descriptor->type() == teBlock->listOfDescriptors()->at(i)->type())
      i++;
    // and append the descriptor:
    teBlock->listOfDescriptors()->insert(i, descriptor);
    teBlock->setIsFormatModified(true);
    tcBlock->descriptorByLCode()->insert(descriptor->getLCode(), descriptor);
    descriptorByLCode_.insert(descriptor->getLCode(), descriptor);
    numOfModified++;
    delete d;
  };
  logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
    QString().sprintf(": finisFormatModifying(): added %d descriptors",
    numOfModified));
  
  // adjust service records of the format descriptor:
  for (int tcIdx=0; tcIdx<format_->listOfTcBlocks()->size(); tcIdx++)
  {
    SgDbhTcBlock *tcBlock = format_->listOfTcBlocks()->at(tcIdx);
    for (int teIdx=0; teIdx<tcBlock->listOfTeBlocks()->size(); teIdx++)
      tcBlock->listOfTeBlocks()->at(teIdx)->adjustServiceRecords();
  };

  // modify data records:
  // TOC#0:
  updateDataRecords(&listOfDataBlocksToc0_, 0);
  // Observations:
  for (int i=0; i<listOfObservations_.size(); i++)
  {
    SgDbhObservationEntry *e = listOfObservations_.at(i);
    for (int tcIdx=0; tcIdx<e->listOfTcsData().size(); tcIdx++)
      updateDataRecords(e->listOfTcsData().at(tcIdx), tcIdx+1);
  };
  // update offsets of the descriptors:
  updateDescriptorsParameters();
  logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
    ": finisFormatModifying(): data records are adjusted");
  formatState_ = FS_FormatAltered;
};



//
void SgDbhImage::updateDataRecords(QList<SgDbhDataBlock*>* listOfDataBlocks, int tcIdx)
{
  int                           offset=0, sizeOfType=0, recordLength=0;
  SgDbhDataBlock               *dataBlock=NULL, *newDataBlock=NULL;
  QList<SgDbhDatumDescriptor*> *listOfDescriptors=NULL;
  SgDbhPhysicalRecord          *newRecord=NULL, *oldRecord=NULL;
  SgDbhDatumDescriptor::Type    recordType=SgDbhDatumDescriptor::T_UNKN;
  SgDbhTeBlock                 *teBlock=NULL;
  for (int teIdx=0; teIdx<listOfDataBlocks->size(); teIdx++)
  {
    dataBlock = listOfDataBlocks->at(teIdx);
    teBlock = format_->listOfTcBlocks()->at(tcIdx)->listOfTeBlocks()->at(teIdx);
    if (teBlock->isFormatModified())
    {
      listOfDescriptors = teBlock->listOfDescriptors();
      newDataBlock = new SgDbhDataBlock;
      for (int i=0; i<listOfDescriptors->size(); i++)
      {
        SgDbhDatumDescriptor* descriptor=listOfDescriptors->at(i);
        if (descriptor->getLCode()=="R-FILLER")
        {
          offset = 0;
          sizeOfType = sizeof(double);
          recordType = SgDbhDatumDescriptor::T_R8;
          recordLength = sizeOfType*teBlock->recTe().r8Num();
          newRecord = new SgDbhDataRecord<double>;
          newRecord->reSize(recordLength);
          newDataBlock->listOfRecords()->append(newRecord);
          newDataBlock->recordByType()->insert(recordType, newRecord);
          oldRecord = dataBlock->recordByType()->value(recordType);
        }
        else if (descriptor->getLCode()=="I-FILLER")
        {
          offset = 0;
          sizeOfType = sizeof(short);
          recordType = SgDbhDatumDescriptor::T_I2;
          recordLength = sizeOfType*teBlock->recTe().i2Num();
          newRecord = new SgDbhDataRecord<short>;
          newRecord->reSize(recordLength);
          newDataBlock->listOfRecords()->append(newRecord);
          newDataBlock->recordByType()->insert(recordType, newRecord);
          oldRecord = dataBlock->recordByType()->value(recordType);
        }
        else if (descriptor->getLCode()=="A-FILLER")
        {
          offset = 0;
          sizeOfType = 2*sizeof(char);
          recordType = SgDbhDatumDescriptor::T_A2;
          recordLength = sizeOfType*teBlock->recTe().a2Num();
          newRecord = new SgDbhDataRecordString;
          newRecord->reSize(recordLength);
          newDataBlock->listOfRecords()->append(newRecord);
          newDataBlock->recordByType()->insert(recordType, newRecord);
          oldRecord = dataBlock->recordByType()->value(recordType);
        }
        else if (descriptor->getLCode()=="D-FILLER")
        {
          offset = 0;
          sizeOfType = sizeof(double);
          recordType = SgDbhDatumDescriptor::T_D8;
          recordLength = sizeOfType*teBlock->recTe().d8Num();
          newRecord = new SgDbhDataRecord<double>;
          newRecord->reSize(recordLength);
          newDataBlock->listOfRecords()->append(newRecord);
          newDataBlock->recordByType()->insert(recordType, newRecord);
          oldRecord = dataBlock->recordByType()->value(recordType);
        }
        else if (descriptor->getLCode()=="J-FILLER")
        {
          offset = 0;
          sizeOfType = sizeof(int);
          recordType = SgDbhDatumDescriptor::T_J4;
          recordLength = sizeOfType*teBlock->recTe().j4Num();
          newRecord = new SgDbhDataRecord<int>;
          newRecord->reSize(recordLength);
          newDataBlock->listOfRecords()->append(newRecord);
          newDataBlock->recordByType()->insert(recordType, newRecord);
          oldRecord = dataBlock->recordByType()->value(recordType);
        };
        // copy data
        if (descriptor->offset()>-1) // if data exist already, copy it
          memcpy(newRecord->base() + sizeOfType*offset, 
            oldRecord->base() + sizeOfType*descriptor->offset(), 
            sizeOfType*descriptor->dim1()*descriptor->dim2()*descriptor->dim3());
        offset += descriptor->dim1()*descriptor->dim2()*descriptor->dim3();
      };
      // update the list of record:
      dataBlock->recordByType()->clear();               // first, clear the hash
      while (!dataBlock->listOfRecords()->isEmpty())    // then, clear the list
        delete dataBlock->listOfRecords()->takeFirst();
      // move pointers from newDataBlock to dataBlock
      for (int i=0; i<newDataBlock->listOfRecords()->size(); i++)
        dataBlock->listOfRecords()->append(newDataBlock->listOfRecords()->at(i));
      QHash<int, SgDbhPhysicalRecord*>::const_iterator j;
      for (j=newDataBlock->recordByType()->constBegin(); 
          j!=newDataBlock->recordByType()->constEnd(); ++j)
        dataBlock->recordByType()->insert(j.key(), j.value());
      // at last, clear the temporary data storage:
      newDataBlock->recordByType()->clear();
      newDataBlock->listOfRecords()->clear();
      delete newDataBlock;
    };
  };
};



//
void SgDbhImage::updateDescriptorsParameters()
{
  int                           offset=0;
  QList<SgDbhDatumDescriptor*>  *listOfDescriptors=NULL;
  SgDbhTcBlock                  *tcBlock = NULL;
  SgDbhTeBlock                  *teBlock = NULL;
  for (int tcIdx=0; tcIdx<format_->listOfTcBlocks()->size(); tcIdx++)
  {
    tcBlock = format_->listOfTcBlocks()->at(tcIdx);
    for (int teIdx=0; teIdx<tcBlock->listOfTeBlocks()->size(); teIdx++)
    {
      teBlock = tcBlock->listOfTeBlocks()->at(teIdx);
      listOfDescriptors = teBlock->listOfDescriptors();
      for (int i=0; i<listOfDescriptors->size(); i++)
      {
        SgDbhDatumDescriptor* descriptor=listOfDescriptors->at(i);
        if (descriptor->getLCode()=="R-FILLER")
          offset = 0;
        else if (descriptor->getLCode()=="I-FILLER")
          offset = 0;
        else if (descriptor->getLCode()=="A-FILLER")
          offset = 0;
        else if (descriptor->getLCode()=="D-FILLER")
          offset = 0;
        else if (descriptor->getLCode()=="J-FILLER")
          offset = 0;
        descriptor->setOffset(offset); // update offset
        offset += descriptor->dim1()*descriptor->dim2()*descriptor->dim3();
      };
    };
  };
};



//
void SgDbhImage::writeDataRecords(SgDbhStream& s, 
  const QList<SgDbhDataBlock*>* listOfDBlocks, int tcIdx) const
{
  int numOfWrittenPhysRecords = 0;
  SgDbhServiceRecordDr    recDr;
  SgDbhServiceRecordDe    recDe;
  recDr.setPrefix("DR");
  recDe.setPrefix("DE");
  recDr.reSize(24);
  recDe.reSize(24);
  recDr.setTcNo(tcIdx);
  recDr.setNumOfTeBlocks(listOfDBlocks->size());
  s << recDr;
  numOfWrittenPhysRecords++;
  for (int teIdx=0; teIdx<listOfDBlocks->size(); teIdx++)
  {
    SgDbhDataBlock* dataBlock = listOfDBlocks->at(teIdx);
    SgDbhTcBlock *tcBlock = format_->listOfTcBlocks()->at(tcIdx);
    SgDbhTeBlock *teBlock = tcBlock->listOfTeBlocks()->at(teIdx);
    recDe.setTeNo(teIdx);
    recDe.setNumOfR8(teBlock->recTe().r8Num());
    recDe.setNumOfI2(teBlock->recTe().i2Num());
    recDe.setNumOfA2(teBlock->recTe().a2Num());
    recDe.setNumOfD8(teBlock->recTe().d8Num());
    recDe.setNumOfJ4(teBlock->recTe().j4Num());
    s << recDe;
    numOfWrittenPhysRecords++;
    for (int i=0; i<dataBlock->listOfRecords()->size(); i++)
    {
      s << *dataBlock->listOfRecords()->at(i);
      numOfWrittenPhysRecords++;
    };
  };
  recDe.setAltered();
  recDe.setNumOfPhRecs(numOfWrittenPhysRecords);
  s << recDe;
};



//
bool SgDbhImage::addDescriptor(SgDbhDatumDescriptor* newDescr)
{
  bool                          isOK=true;
  if (formatState_!=FS_FormatAltering)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      ": addDescriptor(): called when the image is not in proper state");
    return false;
  };
  if (!newDescr)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      ": addDescriptor(): descriptor is NULL");
    return false;
  };
  
  // copy input info:
  SgDbhDatumDescriptor         *d=new SgDbhDatumDescriptor;
  *d = *newDescr;
  // check if it is already in the lists:
  if (descriptorByLCode_.contains(d->getLCode()))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      QString().sprintf(": addDescriptor(): the descriptor [%s] is already in the list",
      qPrintable(d->getLCode())));
    return false;
  };
  // check parameters of the descriptor:
  if (d->type() == SgDbhDatumDescriptor::T_UNKN)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      QString().sprintf(": addDescriptor(): the descriptor [%s] has unknown type",
      qPrintable(d->getLCode())));
    return false;
  };
  if (d->dim1()*d->dim2()*d->dim3() == 0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      QString().sprintf(": addDescriptor(): the descriptor [%s] has wrong dimensions (%d,%d,%d)",
      qPrintable(d->getLCode()), d->dim1(), d->dim2(), d->dim3()));
    return false;
  };
  //
  // adjust what is possible:
  d->setModifiedAtVersion(currentVersion_);
  //
  if (d->nTc()<0) // ok, if nTc==0 -- that is the TOC#0.
  {
    // find out a TOC with smallest number of parameters
    // (perhaps, later we could got other criteria)
    int minSize=format_->listOfTcBlocks()->at(1)->descriptorByLCode()->size(), tcMinSize=-1;
    for (int tcCount=1; tcCount<format_->listOfTcBlocks()->size(); tcCount++)
      if (minSize>=format_->listOfTcBlocks()->at(tcCount)->descriptorByLCode()->size())
      {
        minSize = format_->listOfTcBlocks()->at(tcCount)->descriptorByLCode()->size();
        tcMinSize = tcCount;
      };
    d->setNTc(tcMinSize);
  }
  else if (format_->listOfTcBlocks()->size()<=d->nTc()) // check TC index
  {
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() + ": addDescriptor(): " +
      QString().sprintf("the TOC number of the descriptor [%s] was adjusted from %d to %d",
      qPrintable(d->getLCode()), d->nTc(), format_->listOfTcBlocks()->size()-1));
    d->setNTc(format_->listOfTcBlocks()->size() - 1);
  };
  //
  SgDbhTcBlock                 *tcBlock=format_->listOfTcBlocks()->at(d->nTc());
  if (d->nTe()<0)
  {
    // find out a te block with smallest number of parameters
    // (perhaps, later we could got other criteria)
    int                         minSize, teMinSize;
    minSize = tcBlock->listOfTeBlocks()->at(0)->listOfDescriptors()->size(); 
    teMinSize = -1;
    for (int teCount=0; teCount<tcBlock->listOfTeBlocks()->size(); teCount++)
      if (minSize>=tcBlock->listOfTeBlocks()->at(teCount)->listOfDescriptors()->size())
      {
        minSize = tcBlock->listOfTeBlocks()->at(teCount)->listOfDescriptors()->size();
        teMinSize = teCount;
      };
    d->setNTe(teMinSize);
  }
  else if (tcBlock->listOfTeBlocks()->size()<=d->nTe()) // check TE index
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      QString().sprintf(": addDescriptor(): the descriptor [%s] has wrong TE number %d",
      qPrintable(d->getLCode()), d->nTe()));
    return false;
  };
  //
  // and offset will be calculated later  
  listOfNewDescriptors_.append(d);
  return isOK;
};



//
bool SgDbhImage::delDescriptor(SgDbhDatumDescriptor* delDescr)
{
  bool  isOK = true;
  if (formatState_!=FS_FormatAltering)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      ": delDescriptor(): called when the image is not in proper state");
    return false;
  };
  if (!delDescr)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      ": delDescriptor(): descriptor is NULL");
    return false;
  };

  // copy input info:
  SgDbhDatumDescriptor* d = new SgDbhDatumDescriptor;
  *d = *delDescr;
  // check if it is already in the lists:
  if (!descriptorByLCode_.contains(d->getLCode()))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      QString().sprintf(": delDescriptor(): the descriptor [%s] is not a member of the list",
      qPrintable(d->getLCode())));
    return false;
  };
  if (d->nTc()>=0) // user specified the TOC number, check it:
  {
    if (format_->listOfTcBlocks()->size()<=d->nTc())
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        QString().sprintf(": delDescriptor(): the specified nTc (%d) is less than"
        " the number of TOCs (%d) for the parameter [%s]",
        d->nTc(), format_->listOfTcBlocks()->size(), qPrintable(d->getLCode())));
      return false;
    };
    if (!format_->listOfTcBlocks()->at(d->nTc())->descriptorByLCode()->contains(d->getLCode()))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        QString().sprintf(": delDescriptor(): the descriptor [%s] is not a member of the TOC#%d",
        qPrintable(d->getLCode()), d->nTc()));
      return false;
    };
  };

  listOfDeletedDescriptors_.append(d);
  return isOK;
};



//
void SgDbhImage::dumpFormat()
{
  *dumpStream_ << "== Format dumping:" << endl;
  for (int tcIdx=0; tcIdx<format_->listOfTcBlocks()->size(); tcIdx++)
  {
    SgDbhTcBlock *tcBlock = format_->listOfTcBlocks()->at(tcIdx);
    tcBlock->dump(*dumpStream_);
  };
  *dumpStream_ << "== Format dumped." << endl;
};



//
void SgDbhImage::prepare2save()
{
  // make start block correct for new version:
  startBlock_->rotateVersion(currentVersion_, 
          canonicalName_ + QString().sprintf("_V%03d", currentVersion_));
  // for test purposes:
  if (isSessionCodeAltered_)
    startBlock_->alternateCode(alterCode_);
  // revise the history:
  while (history_.last()->getVersion() == currentVersion_)
    delete history_.takeLast();
};
/*=====================================================================================================*/




/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
SgDbhStream &operator>>(SgDbhStream& s, SgDbhImage& image)
{
  SgDbhDatumDescriptor         *d=NULL;
  SgDbhFormat                  &F=*image.format_;
  s.setByteOrder(QDataStream::BigEndian);
  //
  // initial block:
  s >> *image.startBlock_;
  if (!image.startBlock_->isOk())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_DBH,
      "DBH I/O: the start block is wrong; ignoring file");
    return s;
  };
  if (image.dumpStream_)
    image.startBlock_->dump(*image.dumpStream_);
  //
  // history block:
  image.clearHistoryList();
  SgDbhHistoryEntry            *historyEntry=new SgDbhHistoryEntry;
  if (image.dumpStream_)
    *image.dumpStream_ << "== History Block dump: ==" << endl;
  while (!historyEntry->isLast() && historyEntry->isOk())
  {
    s >> *historyEntry;
    if (historyEntry->isHistoryLine())
    {
      if (image.dumpStream_) 
        historyEntry->dump(*image.dumpStream_);
      image.history_ << historyEntry;
      historyEntry = new SgDbhHistoryEntry;
    };
  };
  if (historyEntry->isLast() && image.dumpStream_)
    *image.dumpStream_ << "== End of dump ==" << endl;
  delete historyEntry;

  F.setDumpStream(image.dumpStream_);
  s >> F;
  if (image.dumpStream_ && F.isOk())
    F.dump(*image.dumpStream_);
  //
  // TOC #1: general stuff for the session:
  F.getBlock(s);
  // save it
  image.copyToc0Content();
  // get info about declared number of observations:
  int                           declaredNumberOfObservations=-1;
  d = image.lookupDescriptor("NUM4 OBS"); 
  if (d)
    declaredNumberOfObservations = image.getJ4(d, 0,0,0);
  else
  {
    d = image.lookupDescriptor("NUMB OBS"); 
    if (d)
      declaredNumberOfObservations = image.getI2(d, 0,0,0);
  };
  //
  int                           processedTOC=F.listOfTcBlocks()->size() + 1;
  int                           observationNumber=-1;
  SgDbhObservationEntry        *obsEntry=NULL;
  while (!s.atEnd() && F.isOk())
  {
    // TOCs #2,3,..:
    F.getBlock(s);
    if (F.isOk())
    {
      if (processedTOC >= F.currentTcNumber()) // new obs
      {
        obsEntry = new SgDbhObservationEntry;
        observationNumber++;
        image.listOfObservations_.append(obsEntry);
      };
      obsEntry->saveDataBlocksFromTcBlock((processedTOC=F.currentTcNumber()), *F.currentTcBlock());
    }; 
  };
  //
  // general setups:
  image.contentState_ = SgDbhImage::CS_DataFromFile;
  image.formatState_ = SgDbhImage::FS_FormatFromFile;
  image.currentVersion_ = image.startBlock_->version() + 1;
  image.canonicalName_ = image.startBlock_->dbName().left(9);
  // undo contentState flag:
  image.contentState_ = SgDbhImage::CS_DataFromFile;
  //
  // check what we have get and what expected:
  if (declaredNumberOfObservations>0 && 
      declaredNumberOfObservations!=image.listOfObservations_.size()) // complain:
        logger->write(SgLogger::ERR, SgLogger::IO_DBH,
          QString().sprintf("DBH I/O: The number of read observations (%d) differs from the"
          " the declared number in the TOC#0 (%d)",
          image.listOfObservations_.size(), declaredNumberOfObservations));
  else // either no information or the numbers are equal:
    logger->write(SgLogger::INF, SgLogger::IO_DBH,
      QString().sprintf("DBH I/O: read %d observations",
      image.listOfObservations_.size()));
  return s;
};



//
SgDbhStream &operator<<(SgDbhStream& s, const SgDbhImage& image)
{
  SgDbhServiceRecordDr          recDr;
  SgDbhServiceRecordDe          recDe;
  SgDbhFormat                  &F=*image.format_;
  s.setByteOrder(QDataStream::BigEndian);
  recDr.setPrefix("DR");
  recDe.setPrefix("DE");
  recDr.reSize(24);
  recDe.reSize(24);
  //
  // initial block:
  s << *image.startBlock_;
  //
  // history block:
  SgDbhHistoryEntry            *historyEntry;
  for (int i=0; i<image.history_.size(); i++)
  {
    historyEntry = image.history_.at(i);
    s << *historyEntry;
  };
  // the last is ZZ record:
  historyEntry = new SgDbhHistoryEntry;
  historyEntry->unsetOkFlag();
  s << *historyEntry;
  delete historyEntry;
  //
  // save format description:
  s << F;
  //
  // write data:
  // TOC#0:
  image.writeDataRecords(s, &image.listOfDataBlocksToc0_, 0);
  // Observations:
  for (int i=0; i<image.listOfObservations_.size(); i++)
  {
    SgDbhObservationEntry      *e=image.listOfObservations_.at(i);
    for (int tcIdx=0; tcIdx<e->listOfTcsData().size(); tcIdx++)
      image.writeDataRecords(s, e->listOfTcsData().at(tcIdx), tcIdx+1);
  };
  //
  return s;
};
//






/*=====================================================================================================*/
//
// aux functions:
//


// i/o:


/*=====================================================================================================*/
//
// constants:
//


/*=====================================================================================================*/
