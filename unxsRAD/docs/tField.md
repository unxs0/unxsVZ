## tField documentation

### cOtherOptions

#### `CONCAT:cFieldName1,cFieldName2;`

If the cOtherOptions contains the above for a given field then the created table.c file will on insert and update
set the field as: `CONCAT(cFieldName1,' ',cFieldName2)`

This is useful for creating the default cLabel field from
other column data. 

Technical: Implemented in unxsrad.c for `funcModuleUpdateQuery` and `funcModuleInsertQuery`.

#### `FooTable:Filterable`

Adds the field to the FooTable filter select.

#### `FooTable:Report:tNull;`

Technical:

Implemented in unxsrad.c for:

 1. `funcBootstrapFindFields` skips the field.
 1. `funcBootstrapValueFields` skips the field.
 1. `funcBootstrapEditorFields` skips the fiels.
 1. `funcBootstrapRowVars` creates special data html link using glyphicon glyphicon-edit.
 1. `funcBootstrapRowReportVars` creates special data html report link.
 1. `funcBootstrapColsReport` sets field type to html.
