## tField documentation
### cOtherOptions
#### `CONCAT:cFieldName1,cFieldName2;`
If the cOtherOptions contains the above for a given field then the created table.c file will on insert and update
set the field as: `CONCAT(cFieldName1,' ',cFieldName2)` this is useful for creating the default cLabel field from
other column data. 
