// A minimal html window manager
let windows = []
let root = undefined
let windowWidth = "250px";
let windowheight = "150px";
let resizeHandleSize = 15
var currentMaxZ = 0;

class QwmWindow {
    constructor(x, y) {
        
        // Hardcode top-level windows manager root to be the document root.
        if (root === undefined)
            root = document.body;

        // Window div structure
        // windowContainer
        // |-windowTitleContainer
        //   |-windowTitle
        //   |-windowClose
        // |-windowContentContainer
        //   |-windowUserContent
        // |-windowResie

        // top-level window container
        this.windowContainer = document.createElement("div");
        setStyles(this.windowContainer, {
            visibility : "hidden",
            position : "absolute",
            display : "flex",
            flexDirection : "column",
            left : x + "px",
            top : y + "px",
            width : windowWidth,
            height : windowheight,
            background : "white",
            boxShadow  : "0 0 5px 5px rgba(0, 0, 0, 0.4)",
            zIndex : currentMaxZ++
        });

        // window title area container, implements window drag
        this.windowTitleContainer = document.createElement("div");
        this.windowContainer.appendChild(this.windowTitleContainer);
        setStyles(this.windowTitleContainer, {
            display : "flex",
            flexDirection : "row",
            height : "35px",
            backgroundColor : "#465dA8",
        });
        installDragHander(this, this.windowTitleContainer, (deltaX, deltaY) => {
            this.windowContainer.style.left = parseFloat(this.windowContainer.style.left) + deltaX + "px";
            this.windowContainer.style.top = parseFloat(this.windowContainer.style.top) + deltaY + "px";
            if (this.geometryUpdateCallback !== undefined)
                this.geometryUpdateCallback();
        })

        // window title text
        this.windowTitle = document.createElement("div");
        this.windowTitleContainer.appendChild(this.windowTitle);
        setStyles(this.windowTitle, {
            display : "flex",
            flex : "1",
            alignItems : "center",
            paddingLeft : "10px",
            paddingRigth : "10px",
            color : "white",
            fontFamily: "Sans-serif",
            userSelect : false,
            cursor : "default",
            unselectable : "on",
            innerText  : "Window",
        });

        // window close button
        this.windowClose = document.createElement("button");
        this.windowTitleContainer.appendChild(this.windowClose);
        this.windowClose.innerText = "X";
        setStyles(this.windowClose, {
            display : "flex",
            width : "1px",
            height : "20px",
        });
        this.windowClose.addEventListener("click", () => { this.close(); });

        // window content container
        this.windowContentContainer = document.createElement("div");
        this.windowContainer.appendChild(this.windowContentContainer);
        setStyles(this.windowContentContainer, {
            flex : 1,
            minHeight: "60px",
            zIndex : 0
        });

        // window resize area
        this.windowResizeButton = document.createElement("div");
        this.windowContainer.appendChild(this.windowResizeButton);
        setStyles(this.windowResizeButton, {
            position : "absolute",
            backgroundColor : "#FFFFFF50",
            left : (parseFloat(this.windowContainer.style.width) - resizeHandleSize) + "px",
            top : (parseFloat(this.windowContainer.style.height) - resizeHandleSize) + "px",
            width : resizeHandleSize + "px",
            height : resizeHandleSize + "px",
            zIndex : 1
        });
        installDragHander(this, this.windowResizeButton, (deltaX, deltaY) => {
            let newWidth = Math.max(parseFloat(windowWidth), parseFloat(this.windowContainer.style.width) + deltaX);
            let newHeight = Math.max(parseFloat(windowheight), parseFloat(this.windowContainer.style.height) + deltaY);
            this.windowContainer.style.width = newWidth + "px";
            
            this.windowContainer.style.height = newHeight + "px";
            this.windowResizeButton.style.left = newWidth - resizeHandleSize + "px";
            this.windowResizeButton.style.top = newHeight - resizeHandleSize + "px";
            if (this.geometryUpdateCallback !== undefined)
                this.geometryUpdateCallback();
        });

        windows.push(this.windowContainer);
        root.append(this.windowContainer);

        function setStyles(element, styles) {
            for (var style in styles) {
                element.style[style] = styles[style];
            }
        }

        function installDragHander(window, element, callback) {
            element.onpointerdown = function(event) {
                this.lastMouseX = event.pageX;
                this.lastMouseY = event.pageY;
                this.setPointerCapture(event.pointerId);
                window.windowContainer.style.zIndex = currentMaxZ++;
            }
            element.onpointermove = function(event) {
                if (this.lastMouseX === undefined)
                    return;
                let deltaX = event.pageX - this.lastMouseX;
                let deltaY = event.pageY - this.lastMouseY;
                this.lastMouseX = event.pageX;
                this.lastMouseY = event.pageY;
                callback(deltaX, deltaY)
            }
            element.onpointerup = function(event) {
                this.lastMouseX = undefined;
                this.lastMouseY = undefined;
                this.releasePointerCapture(event.pointerId);
            }
        }
    }

    onClose(callback) {
        this.closeCallback = callback;
    }

    close() {

        console.log("close", );

        windows.splice(windows.indexOf(this.windowContainer), 1);
        root.removeChild(this.windowContainer);
        if (this.closeCallback !== undefined)
            this.closeCallback();
    }

    onGeometryUpdate(callback) {
        this.geometryUpdateCallback = callback;
    }

    get visible() {
        return this.windowContainer.style.visibility == "hidden";
    }

    set visible(visible) {
        this.windowContainer.style.visibility = visible ? "visible" : "hidden"
        console.log("visible " + visible + " " + this.windowContainer.style.visibility)
    }

    get title() {
        return this.windowTitle.innerText;
    }

    set title(title) {
        this.windowTitle.innerText = title;
    }

    get content() {
        return this.windowContentContainer.firstChild;
    }

    set content(content) {
        if (this.windowContentContainer.firstChild && this.windowContentContainer.firstChild === content)
            return;
        if (this.windowContentContainer.firstChild)
            this.windowContentContainer.removeChild(this.windowContentContainer.firstChild);
        this.windowContentContainer.appendChild(content);
    }
}
