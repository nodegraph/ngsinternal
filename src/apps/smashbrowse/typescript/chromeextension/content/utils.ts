class Utils {

    //----------------------------------------------------------------------------------------
    //Arrays.
    //----------------------------------------------------------------------------------------

    //Returns true if the contents of two arrays of primitives are equal.
    static arrays_are_equal<T>(a1: T[], a2: T[]): boolean {
        if (a1.length != a2.length) {
            return false
        }
        for (var i = 0; i < a1.length; i++) {
            if (a1[i] != a2[i]) {
                return false
            }
        }
        return true
    }

    static value_is_more_extreme(value: number, extreme: number, side: DirectionType) {
        switch (side) {
            case DirectionType.kLeft:
                if (value < extreme) {
                    return true
                }
                break
            case DirectionType.kRight:
                if (value > extreme) {
                    return true
                }
                break
            case DirectionType.kUp:
                if (value < extreme) {
                    return true
                }
                break
            case DirectionType.kDown:
                if (value > extreme) {
                    return true
                }
                break
            default:
                console.log("Error: value_is_more_extreme(value, extreme, side) was given an unknown side argument")
        }
        return false
    }

    //----------------------------------------------------------------------------------------
    //String Processing.
    //----------------------------------------------------------------------------------------

    //Returns true if the text is composed of all whitespace characters.
    static is_all_whitespace(text: string): boolean {
        let regex = /^\s+$/;
        return regex.test(text)
    }

    static strip_quotes(text: string): string {
        if (text.length == 0) {
            return text
        }
        if ((text[0] == '"') && (text[text.length - 1] == '"')) {
            return text.slice(1, -1)
        }
        if ((text[0] == "'") && (text[text.length - 1] == "'")) {
            return text.slice(1, -1)
        }
        return text
    }

}





