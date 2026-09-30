// Tomasz Podlaszewski

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct cnumb // structure that holds colossal numbers.
{
    struct cnumb *e;
    struct cnumb *next;
} cnumb;

typedef struct stack // normal implementation of a stack, that holds cnumbers intstead of integers.
{
    struct stack *under;
    cnumb *v;
} stack;

void free_cnumb(cnumb *a) // function to dealocate memory of a colossal number.
{
    if (a == NULL)
        return;

    free_cnumb(a->e);
    free_cnumb(a->next);
    free(a);
}

bool empty(stack *s)
{
    return (s == NULL);
}

void init(stack **s)
{
    *s = NULL;
}

cnumb *top(stack *s)
{
    return s->v;
}

void push(stack **s, cnumb *value)
{
    stack *temp = (stack *)malloc(sizeof(stack));
    temp->under = *s;
    temp->v = value;
    *s = temp;
}

cnumb *pop(stack **s)
{
    if (empty(*s))
        return NULL;
    cnumb *x = (*s)->v;
    stack *pom = *s;
    *s = (*s)->under;
    free(pom);
    return x;
}

void delete(stack **s)
{
    cnumb *dummy;

    while (!empty(*s))
    {
        dummy = pop(s);
        free_cnumb(dummy);
    }
    free(*s);
}

int compare(cnumb *a, cnumb *b) // compares 2 normalised numbers.
{
    if (a == b)
        return 0;
    else if (a == NULL)
        return -1;
    else if (b == NULL)
        return 1;

    int result = compare(a->e, b->e);
    if (result != 0)
        return result;
    else
        return compare(a->next, b->next);
}

cnumb *copy(cnumb *a)
{
    if (a == NULL)
        return NULL;

    cnumb *new_node = (cnumb *)malloc(sizeof(cnumb));
    new_node->e = copy(a->e);
    new_node->next = copy(a->next);

    return new_node;
}

cnumb *create_one() // creates colossal number equal to 1, used in adding 2 numbers with equal exponents.
{
    cnumb *one = (cnumb *)malloc(sizeof(cnumb));

    one->e = NULL;
    one->next = NULL;

    return one;
}

cnumb *add(cnumb *a, cnumb *b) // ads two numbers, outputs their normalised sum and frees them.
{
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    int comp = compare(a->e, b->e);

    if (comp > 0) // a is bigger than b.
    {
        a->next = add(a->next, b);

        if (a->next != NULL && compare(a->e, a->next->e) == 0) // first term in rest of the sum has the same exponent as a, so we have to merge them.
        {
            cnumb *duplicate = a->next;
            a->next = duplicate->next;

            free_cnumb(duplicate->e);
            free(duplicate);

            cnumb *one = create_one();
            a->e = add(a->e, one);

            cnumb *rest = a->next;
            a->next = NULL;
            return add(a, rest);
        }

        return a;
    }

    else if (comp < 0) // b is bigger.
    {
        b->next = add(a, b->next);

        if (b->next && compare(b->e, b->next->e) == 0) // same as before.
        {
            cnumb *duplicate = b->next;
            b->next = duplicate->next;

            free_cnumb(duplicate->e);
            free(duplicate);

            cnumb *one = create_one();
            b->e = add(b->e, one);

            cnumb *rest = b->next;
            b->next = NULL;
            return add(b, rest);
        }

        return b;
    }

    else // equal exponents.
    {
        cnumb *rest_b = b->next;
        free_cnumb(b->e);
        free(b);

        cnumb *rest = add(a->next, rest_b);

        cnumb *one = create_one();
        a->e = add(a->e, one);

        a->next = NULL;
        return add(a, rest); // we have to use add, because there might be a term with the same exponent as a
    }
}

cnumb *mult_scalar(cnumb *a, cnumb *term_b) // helper function, multiplies a by a single term (2^exp).
{
    if (a == NULL)
        return NULL;

    cnumb *head = NULL;
    cnumb **ptr = &head;

    for (cnumb *curr = a; curr != NULL; curr = curr->next)
    {
        cnumb *new_node = (cnumb *)malloc(sizeof(cnumb));

        new_node->e = add(copy(curr->e), copy(term_b->e)); // we have to copy exponents, because add() frees the originals.
        new_node->next = NULL;

        *ptr = new_node;
        ptr = &(new_node->next);
    }
    return head;
}

cnumb *multiply(cnumb *a, cnumb *b) // multiplies two numbers.
{
    if (a == NULL || b == NULL) // NULL means 0 in my implementation.
        return NULL;

    size_t len_b = 0;
    for (cnumb *t = b; t != NULL; t = t->next)
        len_b++;

    cnumb **partials = (cnumb **)malloc(len_b * sizeof(cnumb *));

    cnumb *curr_b = b;
    for (size_t i = 0; i < len_b; i++)
    {
        partials[i] = mult_scalar(a, curr_b);
        curr_b = curr_b->next;
    }

    size_t size = len_b;
    while (size > 1)
    {
        size_t new_size = 0;
        for (size_t i = 0; i < size; i += 2)
        {
            if (i + 1 < size)
            {
                partials[new_size++] = add(partials[i], partials[i + 1]);
            }
            else
            {
                partials[new_size++] = partials[i];
            }
        }
        size = new_size;
    }

    cnumb *result = partials[0];
    free(partials);

    return result;
}

cnumb *read(int c) // function to read the input.
{

    while (c == '\n' || c == ' ')
    {
        c = getchar();
        if (c == EOF)
            return NULL;
    }

    if (c == '0')
    {
        return NULL; // end of a given number.
    }
    else if (c == '1')
    {
        c = getchar();
        cnumb *exponent = read(c);

        cnumb *current_term = (cnumb *)malloc(sizeof(cnumb));
        current_term->e = exponent;
        current_term->next = NULL;

        c = getchar();
        cnumb *rest_of_number = read(c);

        return add(current_term, rest_of_number);
    }
    return NULL;
}

void write(cnumb *n)
{
    if (n == NULL)
        printf("0");
    else
    {
        printf("1");
        write(n->e);
        write(n->next);
    }
}

cnumb *create_exp(cnumb *a)
{
    cnumb *result = (cnumb *)malloc(sizeof(cnumb));
    result->e = a;
    result->next = NULL;

    return result;
}

int main()
{
    stack *s;
    init(&s);

    int c = getchar();

    while (c != EOF)
    {

        if (c == '1' || c == '0')
        {
            push(&s, read(c));
        }

        else if (c == '.')
        {
            cnumb *curr = pop(&s);

            write(curr);
            printf("\n");

            free_cnumb(curr);
        }

        else if (c == ':')
            push(&s, copy(top(s)));

        else if (c == '^')
        {
            cnumb *a = pop(&s);
            cnumb *result = create_exp(a);
            push(&s, result);
        }

        else if (c == '+')
        {
            cnumb *a = pop(&s);
            cnumb *b = pop(&s);

            cnumb *result = add(a, b); // add() frees a and b.

            push(&s, result);
        }

        else if (c == '*')
        {
            cnumb *a = pop(&s);
            cnumb *b = pop(&s);

            cnumb *result = multiply(a, b);
            push(&s, result);

            free_cnumb(a);
            free_cnumb(b);
        }

        c = getchar();
    }

    delete(&s);
    return 0;
}