/*
 * FALCON - The Falcon Programming Language.
 * FILE: dbi_ext.h
 *
 * DBI Falcon extension interface
 * -------------------------------------------------------------------
 * Author: Giancarlo Niccolai and Jeremy Cowgar
 * Begin: Sun, 23 Dec 2007 22:02:37 +0100
 *
 * -------------------------------------------------------------------
 * (C) Copyright 2007: the FALCON developers (see list in AUTHORS file)
 *
 * See LICENSE file for licensing details.
 */

#include <falcon/setup.h>
#include <falcon/types.h>

#ifndef DBI_EXT_H
#define DBI_EXT_H

namespace Falcon {

class VMachine;

namespace Ext {

//=====================
// DBI Generic
//=====================

void DBIConnect( VMachine *vm );

//=====================
// DBIBaseTrans
//=====================

void Statement_execute( VMachine *vm );
void Statement_aexec( VMachine *vm );
void Statement_reset( VMachine *vm );
void Statement_close( VMachine *vm );
void Statement_affected(CoreObject *instance, void *user_data, Item &property, const PropEntry& entry );

//=====================
// DBI Handle
//=====================

void Handle_query( VMachine *vm );
void Handle_result( VMachine *vm );
void Handle_aquery( VMachine *vm );
void Handle_options( VMachine *vm );
void Handle_prepare( VMachine *vm );
void Handle_getLastID( VMachine *vm );
void Handle_close( VMachine *vm );

void Handle_begin( VMachine *vm );
void Handle_commit( VMachine *vm );
void Handle_rollback( VMachine *vm );
void Handle_lselect( VMachine *vm );
void Handle_expand( VMachine *vm );
void Handle_affected(CoreObject *instance, void *user_data, Item &property, const PropEntry& entry );

//=====================
// DBI Recordset
//=====================

void Recordset_discard( VMachine *vm );
void Recordset_fetch( VMachine *vm );
void Recordset_do( VMachine *vm );
void Recordset_next( VMachine *vm );

void Recordset_getCurrentRow( VMachine *vm );
void Recordset_getRowCount( VMachine *vm );
void Recordset_getColumnCount( VMachine *vm );
void Recordset_getColumnNames( VMachine *vm );
void Recordset_close( VMachine *vm );

//=====================
// DBI Error class
//=====================

void DBIError_init( VMachine *vm );

}
}

#endif

/* end of dbi_ext.h */

