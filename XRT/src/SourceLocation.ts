interface _SourceLocation {
    file_name?: string
    file_path?: string
    line?: number
    column?: number
    entry_size?: number
}

export class SourceLocation {
    file_name?: string
    file_path?: string
    line?: number
    column?: number
    entry_size?: number

    constructor(private _src: _SourceLocation) {
        this.file_name = _src.file_name
        this.file_path = _src.file_path
        this.line = _src.line
        this.column = _src.column
        this.entry_size = _src.entry_size
    }
}