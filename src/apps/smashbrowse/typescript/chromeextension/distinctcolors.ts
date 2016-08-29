// We use a predefined set of distinct colors.
// These were obtained from stack overflow.
// http://stackoverflow.com/questions/470690/how-to-automatically-generate-n-distinct-colors

interface ColorInfo {
    color: string 
    index: number
}

class DistinctColors {
    //Red is reserved for text.
    static text_color = "#FF0000" 
    //Blue is reserved for images.
    static image_color = "#0000FF" 
    //A set of distinct colors to indicate separate sets.
    static usable_colors = [
        "#00FF00", // green
        "#FFFF00", // yellow
        "#00FFFF", // turqoise
        "#FF00FF", // purple
        // The following were obtained from stack overflow.
        "#023FA5",
        "#7D87B9",
        "#BEC1D4",
        "#D6BCC0",
        "#BB7784",
        "#FFFFFF",
        "#4A6FE3",
        "#8595E1",
        "#B5BBE3",
        "#E6AFB9",
        "#E07B91",
        "#D33F6A",
        "#11C638",
        "#8DD593",
        "#C6DEC7",
        "#EAD3C6",
        "#F0B98D",
        "#EF9708",
        "#0FCFC0",
        "#9CDED6",
        "#D5EAE7",
        "#F3E1EB",
        "#F6C4E1",
        "#F79CD4"
    ]
    
    // Our members.
    used_colors: { [color: string]: boolean };
    
    // Constructor.
    constructor() {
        this.reset()
    }
    
    reset() {
        this.used_colors = {
            "#00FF00": false, // green
            "#FFFF00": false, // yellow
            "#00FFFF": false, // turqoise
            "#FF00FF": false, // purple
            // The following were obtained from stack overflow.
            "#023FA5": false,
            "#7D87B9": false,
            "#BEC1D4": false,
            "#D6BCC0": false,
            "#BB7784": false,
            "#FFFFFF": false,
            "#4A6FE3": false,
            "#8595E1": false,
            "#B5BBE3": false,
            "#E6AFB9": false,
            "#E07B91": false,
            "#D33F6A": false,
            "#11C638": false,
            "#8DD593": false,
            "#C6DEC7": false,
            "#EAD3C6": false,
            "#F0B98D": false,
            "#EF9708": false,
            "#0FCFC0": false,
            "#9CDED6": false,
            "#D5EAE7": false,
            "#F3E1EB": false,
            "#F6C4E1": false,
            "#F79CD4": false
        }
    }

    //Checkout/obtain a color to use.
    obtain_color(): ColorInfo {
        for (let i = 0; i < DistinctColors.usable_colors.length; i++) {
            let color = DistinctColors.usable_colors[i]
            if (this.used_colors[color] == false) {
                this.used_colors[color] = true
                return { color: color, index: i }
            }
        }
        return {color: "", index: -1}
    }

    //Request to use a color. Returns true if approved.
    request_color(color: string): number {
        // Make sure the color is one that we track.
        let index = DistinctColors.usable_colors.indexOf(color)
        if (index < 0) {
            return -1
        }
        // Make sure it's not currently being used.
        if (this.used_colors[color] == false) {
            this.used_colors[color] = true
            return index
        }
        return -1
    }

    //Release a color so that it can be used again.
    release_color(color: string): void {
        // Make sure the color is one that we track.
        let index = DistinctColors.usable_colors.indexOf(color)
        if (index < 0) {
            return
        }
        // Make sure the color is recorded as not being used.
        this.used_colors[color] = false
    }
}







