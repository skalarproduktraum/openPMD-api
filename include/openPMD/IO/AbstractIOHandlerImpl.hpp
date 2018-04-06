/* Copyright 2018 Fabian Koller
 *
 * This file is part of openPMD-api.
 *
 * openPMD-api is free software: you can redistribute it and/or modify
 * it under the terms of of either the GNU General Public License or
 * the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * openPMD-api is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License and the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * and the GNU Lesser General Public License along with openPMD-api.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "openPMD/IO/IOTask.hpp"

#include <future>


namespace openPMD
{
class AbstractIOHandler;
class Writable;

class AbstractIOHandlerImpl
{
public:
  AbstractIOHandlerImpl(AbstractIOHandler*);
  virtual ~AbstractIOHandlerImpl();

  virtual std::future< void > flush();

  /** Create a new file in physical storage, possibly overriding an existing file.
   *
   * The operation should fail if m_handler->accessType is AccessType::READ_ONLY.
   * The new file should be located in m_handler->directory.
   * The new file should have the filename parameters.name.
   * The filename should include the correct corresponding filename extension.
   * Any existing file should be overwritten if m_handler->accessType is AccessType::CREATE.
   * The Writables file position should correspond to the root group "/" of the hierarchy.
   * The Writable should be marked written when the operation completes successfully.
   */
  virtual void createFile(Writable*, Parameter< Operation::CREATE_FILE > const&) = 0;
  /** Create all necessary groups for a path, possibly recursively.
   *
   * The operation should fail if m_handler->accessType is AccessType::READ_ONLY.
   * The path parameters.path may contain multiple levels (e.g. first/second/third/).
   * The Writables file position should correspond to the complete newly created path (i.e. first/second/third/ should be assigned to the Writables file position).
   * The Writable should be marked written when the operation completes successfully.
   */
  virtual void createPath(Writable*, Parameter< Operation::CREATE_PATH > const&) = 0;
  /** Create a new dataset of given type, extent and storage properties.
   *
   * The operation should fail if m_handler->accessType is AccessType::READ_ONLY.
   * The path may contain multiple levels (e.g. group/dataset).
   * The new dataset should have the name parameters.name. This name should not start or end with a slash ("/").
   * The new dataset should be of datatype parameters.dtype.
   * The new dataset should have an extent of parameters.extent.
   * If possible, the new dataset should be extensible.
   * If possible, the new dataset should be divided into chunks with size parameters.chunkSize.
   * If possible, the new dataset should be compressed according to parameters.compression. This may be format-specific.
   * If possible, the new dataset should be transformed accoring to parameters.transform. This may be format-specific.
   * The Writables file position should correspond to the newly created dataset.
   * The Writable should be marked written when the operation completes successfully.
   */
  virtual void createDataset(Writable*, Parameter< Operation::CREATE_DATASET > const&) = 0;
  /** Increase the extent of an existing dataset.
   *
   * The operation should fail if m_handler->accessType is AccessType::READ_ONLY.
   * The operation should fail if the dataset does not yet exist.
   * The dataset should have the name parameters.name. This name should not start or end with a slash ("/").
   * The operation should fail if the new extent is not strictly large in every dimension.
   * The dataset should have an extent of parameters.extent.
   */
  virtual void extendDataset(Writable*, Parameter< Operation::EXTEND_DATASET > const&) = 0;
  /** Open an existing file assuming it conforms to openPMD.
   *
   * The operation should fail if m_handler->directory is not accessible.
   * The opened file should have filename parameters.name and include the correct corresponding filename extension.
   * The operation should not open files more than once.
   * If possible, the file should be opened with read-only permissions if m_handler->accessType is AccessType::READ_ONLY.
   * The Writables file position should correspond to the root group "/" of the hierarchy in the opened file.
   * The Writable should be marked written when the operation completes successfully.
   */
  virtual void openFile(Writable*, Parameter< Operation::OPEN_FILE > const&) = 0;
  /** Open all contained groups in a path, possibly recursively.
   *
   * The operation should overwrite existing file positions, even when the Writable was already marked written.
   * The path parameters.path may contain multiple levels (e.g. first/second/third/). This path should be relative (i.e. it should not start with a slash "/").
   * The Writables file position should correspond to the complete opened path (i.e. first/second/third/ should be assigned to the Writables file position).
   * The Writable should be marked written when the operation completes successfully.
   */
  virtual void openPath(Writable*, Parameter< Operation::OPEN_PATH > const&) = 0;
  /** Open an existing dataset and determine its datatype and extent.
   *
   * The opened dataset should be located in a group below the group of the Writables parent writable->parent.
   * The opened datasets name should be parameters.name. This name should not start or end with a slash ("/").
   * The opened datasets datatype should be stored in *(parameters.dtype).
   * The opened datasets extent should be stored in *(parameters.extent).
   * The Writables file position should correspond to the opened dataset.
   * The Writable should be marked written when the operation completes successfully.
   */
  virtual void openDataset(Writable*, Parameter< Operation::OPEN_DATASET > &) = 0;
  /** Delete an existing file from physical storage.
   *
   * The operation should fail if m_handler->accessType is AccessType::READ_ONLY.
   * The operation should pass if the Writable was not marked written.
   * All handles that correspond to the file should be closed before deletion.
   * The file to delete should have the filename parameters.name.
   * The filename should include the correct corresponding filename extension.
   * The Writables file position should be set to an invalid position (i.e. the pointer should be a nullptr).
   * The Writable should be marked not written when the operation completes successfully.
   */
  virtual void deleteFile(Writable*, Parameter< Operation::DELETE_FILE > const&) = 0;
  /** Delete all objects within an existing path.
   *
   * The operation should fail if m_handler->accessType is AccessType::READ_ONLY.
   * The operation should pass if the Writable was not marked written.
   * The path parameters.path may contain multiple levels (e.g. first/second/third/). This path should be relative (i.e. it should not start with a slash "/"). It may also contain the current group ".".
   * All groups and datasets starting from the path should not be accessible in physical storage after the operation completes successfully.
   * The Writables file position should be set to an invalid position (i.e. the pointer should be a nullptr).
   * The Writable should be marked not written when the operation completes successfully.
   */
  virtual void deletePath(Writable*, Parameter< Operation::DELETE_PATH > const&) = 0;
  /** Delete an existing dataset.
   *
   * The operation should fail if m_handler->accessType is AccessType::READ_ONLY.
   * The operation should pass if the Writable was not marked written.
   * The dataset should have the name parameters.name. This name should not start or end with a slash ("/"). It may also contain the current dataset ".".
   * The dataset should not be accessible in physical storage after the operation completes successfully.
   * The Writables file position should be set to an invalid position (i.e. the pointer should be a nullptr).
   * The Writable should be marked not written when the operation completes successfully.
   */
  virtual void deleteDataset(Writable*, Parameter< Operation::DELETE_DATASET > const&) = 0;
  /** Delete an existing attribute.
   *
   * The operation should fail if m_handler->accessType is AccessType::READ_ONLY.
   * The operation should pass if the Writable was not marked written.
   * The attribute should be associated with the Writable and have the name parameters.name before deletion.
   * The attribute should not be accessible in physical storage after the operation completes successfully.
   */
  virtual void deleteAttribute(Writable*, Parameter< Operation::DELETE_ATT > const&) = 0;
  /** Write a chunk of data into an existing dataset.
   *
   * The operation should fail if m_handler->accessType is AccessType::READ_ONLY.
   * The dataset should be associated with the Writable.
   * The operation should fail if the dataset does not exist.
   * The operation should fail if the chunk extent parameters.extent is not smaller or equals in every dimension.
   * The operation should fail if chunk positions parameters.offset+parameters.extent do not reside inside the dataset.
   * The dataset should match the dataype parameters.dtype.
   * The data parameters.data is a cast-to-void pointer to a flattened version of the chunk data. It should be re-cast to the provided datatype. The chunk is stored row-major.
   * The region of the chunk should be written to physical storage after the operation completes successully.
   */
  virtual void writeDataset(Writable*, Parameter< Operation::WRITE_DATASET > const&) = 0;
  /** Create a single attribute and fill the value, possibly overwriting an existing attribute.
   *
   * The operation should fail if m_handler->accessType is AccessType::READ_ONLY.
   * The attribute should have the name parameters.name. This name should not contain a slash ("/").
   * The attribute should be of datatype parameters.dtype.
   * Any existing attribute with the same name should be overwritten. If possible, only the value should be changed if the datatype stays the same.
   * The attribute should be written to physical storage after the operation completes successully.
   */
  virtual void writeAttribute(Writable*, Parameter< Operation::WRITE_ATT > const&) = 0;
  /** Read a chunk of data from an existing dataset.
   *
   * The dataset should be associated with the Writable.
   * The operation should fail if the dataset does not exist.
   * The operation should fail if the chunk extent parameters.extent is not smaller or equals in every dimension.
   * The operation should fail if chunk positions parameters.offset+parameters.extent do not reside inside the dataset.
   * The dataset should match the dataype parameters.dtype.
   * The data parameters.data should be a cast-to-void pointer to a flattened version of the chunk data. The chunk should be stored row-major.
   * The region of the chunk should be written to the location indicated by the pointer after the operation completes successully.
   */
  virtual void readDataset(Writable*, Parameter< Operation::READ_DATASET > &) = 0;
  virtual void readAttribute(Writable*, Parameter< Operation::READ_ATT > &) = 0;
  virtual void listPaths(Writable*, Parameter< Operation::LIST_PATHS > &) = 0;
  virtual void listDatasets(Writable*, Parameter< Operation::LIST_DATASETS > &) = 0;
  virtual void listAttributes(Writable*, Parameter< Operation::LIST_ATTS > &) = 0;

  AbstractIOHandler* m_handler;
};  //AbstractIOHandlerImpl
} // openPMD
