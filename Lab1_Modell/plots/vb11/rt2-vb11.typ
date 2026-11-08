
#set text(lang: "de", size: 11pt)

#set page(paper: "a4")

#set document(
  title: [RT2 Lab1],
  author: "o-manoli",
  date: datetime.today()
)

#set page(
  header: stack(
    dir: ttb,
    spacing: 0.5em,
    grid(
      columns: (1fr, 2fr),
      align(left)[Labor Regelungstechnik 2],
      align(right)[
        Vorbereitungsaufgabe 11,
        Versuch Motorboard - Teil 1
      ]
    ),
    line(length: 100%, stroke: 0.5pt)
  )
)

// - -- - -- - -- - -- - -- - -- - --

#let dual-figure-page(fig1, fig2) = layout(size => {

  let cell-height = (size.height - 1cm) / 2

  grid(
    columns: (1fr),
    rows: (cell-height, cell-height),
    gutter: 1cm,

    align(center + horizon)[
      #fig1
    ],

    align(center + horizon)[
      #fig2
    ],
  )
})

// - -- - -- - -- - -- - -- - -- - --

#dual-figure-page(
  figure(
    image("vb11_n.svg"),
    caption: [*Drehzahl* Sollwertsprung von 1000 auf 6000 U/min]
  ),
  figure(
    image("vb11_i.svg"),
    caption: [*Motorstrom* Sollwertsprung von 1000 auf 6000 U/min]
  )
)