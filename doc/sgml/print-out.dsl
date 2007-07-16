<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY dbstyle PUBLIC "-//Norman Walsh//DOCUMENT DocBook Print Stylesheet//EN" CDATA DSSSL>
]>

<style-sheet>
<style-specification use="docbook">
<style-specification-body>

(define %shade-verbatim% #t)


(define %bf-size%
  ;; REFENTRY bf-size
  ;; PURP Defines the body font size
  ;; DESC
  ;; Sets the body font size. This parameter is usually controlled by the
  ;; '%visual-acuity%' parameter.
  ;; /DESC
  ;; /REFENTRY
  (case %visual-acuity%
    (("tiny") 6pt)
    (("normal") 8pt)
    (("presbyopic") 10pt)
    (("large-type") 18pt)))

</style-specification-body>
</style-specification>
<external-specification id="docbook" document="dbstyle">
</style-sheet>

